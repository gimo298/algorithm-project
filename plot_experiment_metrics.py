from __future__ import annotations

import json
import re
from collections import defaultdict
from pathlib import Path
from typing import DefaultDict


RESULT_FILE_PATTERN = re.compile(r"^result_len(\d+)_(\d+)\.json$")
DEFAULT_COLORS = {
    "Trivial": "tab:blue",
    "BWT": "tab:orange",
    "RabinKarp": "tab:green",
    "IndexMapping": "tab:red",
    "KMP": "tab:purple",
}

DEFAULT_MARKERS = {
    "Trivial": "o",
    "BWT": "s",
    "RabinKarp": "^",
    "IndexMapping": "D",
    "KMP": "P",
}

SETTINGS = {
    "version": 3,
    "min_length": 15,
    "max_length": 115,
    "length_step": 5,
    "time_y_min": 0.0,
    "time_y_max": None,
    "time_focus_algorithm": "Trivial",
    "time_zoom_padding_ratio": 0.08,
    "accuracy_y_min": 75.0,
    "accuracy_y_max": 100.0,
    "result_root": Path("Result"),
}


def validate_settings() -> None:
    version = SETTINGS["version"]
    min_length = SETTINGS["min_length"]
    max_length = SETTINGS["max_length"]
    length_step = SETTINGS["length_step"]
    time_y_min = SETTINGS["time_y_min"]
    time_y_max = SETTINGS["time_y_max"]
    accuracy_y_min = SETTINGS["accuracy_y_min"]
    accuracy_y_max = SETTINGS["accuracy_y_max"]

    if version <= 0:
        raise ValueError("version은 1 이상의 정수여야 합니다.")

    if min_length <= 0 or max_length <= 0:
        raise ValueError("길이 범위는 1 이상의 정수여야 합니다.")

    if length_step <= 0:
        raise ValueError("length_step은 1 이상의 정수여야 합니다.")

    if min_length > max_length:
        raise ValueError("min_length는 max_length보다 클 수 없습니다.")

    if (max_length - min_length) % length_step != 0:
        raise ValueError(
            "길이 범위는 length_step 간격으로 나누어 떨어져야 합니다."
        )

    if time_y_max is not None and time_y_min >= time_y_max:
        raise ValueError("time_y_min은 time_y_max보다 작아야 합니다.")

    if accuracy_y_max is not None and accuracy_y_min >= accuracy_y_max:
        raise ValueError("accuracy_y_min은 accuracy_y_max보다 작아야 합니다.")


def load_results(
    version_dir: Path,
    lengths: list[int],
) -> tuple[DefaultDict[str, list[int]], DefaultDict[str, list[float]], DefaultDict[str, list[float]]]:
    x_values: DefaultDict[str, list[int]] = defaultdict(list)
    execution_times: DefaultDict[str, list[float]] = defaultdict(list)
    accuracies: DefaultDict[str, list[float]] = defaultdict(list)

    for read_length in lengths:
        shortreads_dir = version_dir / f"Shortreads_{read_length}"

        if not shortreads_dir.exists():
            continue

        for result_file in sorted(shortreads_dir.glob("result_len*.json")):
            match = RESULT_FILE_PATTERN.match(result_file.name)

            if match is None:
                continue

            file_read_length = int(match.group(1))

            if file_read_length != read_length:
                continue

            with result_file.open("r", encoding="utf-8") as handle:
                payload = json.load(handle)

            algorithms = payload.get("algorithms", {})

            for algorithm_name, stats in algorithms.items():
                execution_time_ms = stats.get("execution_time_ms")
                accuracy = stats.get("accuracy")

                if execution_time_ms is None or accuracy is None:
                    continue

                x_values[algorithm_name].append(read_length)
                execution_times[algorithm_name].append(float(execution_time_ms))
                accuracies[algorithm_name].append(float(accuracy))

    return x_values, execution_times, accuracies


def create_scatter_plot(
    axis,
    x_values: DefaultDict[str, list[int]],
    y_values: DefaultDict[str, list[float]],
    lengths: list[int],
    y_label: str,
    title: str,
    y_min: float | None,
    y_max: float | None,
    algorithm_names: list[str] | None = None,
    x_offsets: dict[str, float] | None = None,
) -> None:
    selected_names = algorithm_names or sorted(x_values.keys())

    for algorithm_name in selected_names:
        if algorithm_name not in x_values:
            continue

        offset = 0.0 if x_offsets is None else x_offsets.get(algorithm_name, 0.0)
        shifted_x_values = [x_value + offset for x_value in x_values[algorithm_name]]

        axis.scatter(
            shifted_x_values,
            y_values[algorithm_name],
            label=algorithm_name,
            color=DEFAULT_COLORS.get(algorithm_name),
            marker=DEFAULT_MARKERS.get(algorithm_name, "o"),
            s=55,
            alpha=0.85,
            edgecolors="white",
            linewidths=0.6,
        )

    axis.set_title(title)
    axis.set_xlabel("Short Read Length")
    axis.set_ylabel(y_label)
    axis.set_xticks(lengths)
    axis.set_xlim(lengths[0] - 1, lengths[-1] + 1)
    axis.set_ylim(bottom=y_min, top=y_max)
    axis.grid(True, linestyle="--", alpha=0.35)

    if axis.collections:
        axis.legend()


def compute_zoomed_time_limits(
    execution_times: DefaultDict[str, list[float]],
    focus_algorithm: str,
    fallback_y_min: float | None,
    fallback_y_max: float | None,
    padding_ratio: float,
) -> tuple[float | None, float | None]:
    non_focus_values = [
        value
        for algorithm_name, values in execution_times.items()
        if algorithm_name != focus_algorithm
        for value in values
    ]

    if not non_focus_values:
        return fallback_y_min, fallback_y_max

    data_min = min(non_focus_values)
    data_max = max(non_focus_values)

    if data_min == data_max:
        padding = max(data_max * padding_ratio, 1.0)
    else:
        padding = (data_max - data_min) * padding_ratio

    zoomed_y_min = max(0.0, data_min - padding)
    zoomed_y_max = data_max + padding

    if fallback_y_min is not None:
        zoomed_y_min = max(fallback_y_min, zoomed_y_min)

    if fallback_y_max is not None:
        zoomed_y_max = min(fallback_y_max, zoomed_y_max)

    return zoomed_y_min, zoomed_y_max


def main() -> None:
    try:
        import matplotlib.pyplot as plt
    except ImportError as exc:
        raise RuntimeError(
            "matplotlib가 설치되어 있지 않습니다. `pip install matplotlib` 후 다시 실행하세요."
        ) from exc

    validate_settings()

    version = SETTINGS["version"]
    min_length = SETTINGS["min_length"]
    max_length = SETTINGS["max_length"]
    length_step = SETTINGS["length_step"]
    result_root = SETTINGS["result_root"]
    time_focus_algorithm = SETTINGS["time_focus_algorithm"]
    time_zoom_padding_ratio = SETTINGS["time_zoom_padding_ratio"]
    lengths = list(range(min_length, max_length + 1, length_step))
    version_dir = result_root / f"DNA_SHORTREAD_ver{version}"

    if not version_dir.exists():
        raise FileNotFoundError(f"{version_dir} 폴더가 존재하지 않습니다.")

    x_values, execution_times, accuracies = load_results(version_dir, lengths)

    if not x_values:
        raise RuntimeError(
            "지정한 범위에서 읽을 수 있는 result_len{length}_{run}.json 파일이 없습니다."
        )

    non_focus_algorithms = sorted(
        algorithm_name
        for algorithm_name in x_values.keys()
        if algorithm_name != time_focus_algorithm
    )

    zoomed_time_y_min, zoomed_time_y_max = compute_zoomed_time_limits(
        execution_times=execution_times,
        focus_algorithm=time_focus_algorithm,
        fallback_y_min=SETTINGS["time_y_min"],
        fallback_y_max=SETTINGS["time_y_max"],
        padding_ratio=time_zoom_padding_ratio,
    )

    time_figure, time_axes = plt.subplots(
        nrows=2,
        figsize=(13, 10),
        sharex=True,
        gridspec_kw={"height_ratios": [3, 2]},
    )

    zoomed_time_axis, focus_time_axis = time_axes

    create_scatter_plot(
        axis=zoomed_time_axis,
        x_values=x_values,
        y_values=execution_times,
        lengths=lengths,
        y_label="Execution Time (ms)",
        title=f"Execution Time vs Short Read Length (Zoomed, ver{version})",
        y_min=zoomed_time_y_min,
        y_max=zoomed_time_y_max,
        algorithm_names=non_focus_algorithms,
    )

    create_scatter_plot(
        axis=focus_time_axis,
        x_values=x_values,
        y_values=execution_times,
        lengths=lengths,
        y_label="Execution Time (ms)",
        title=f"Execution Time for {time_focus_algorithm} (ver{version})",
        y_min=SETTINGS["time_y_min"],
        y_max=SETTINGS["time_y_max"],
        algorithm_names=[time_focus_algorithm],
    )

    accuracy_figure, accuracy_axes = plt.subplots(
        nrows=2,
        figsize=(13, 10),
        sharex=True,
        gridspec_kw={"height_ratios": [3, 2]},
    )

    zoomed_accuracy_axis, focus_accuracy_axis = accuracy_axes

    create_scatter_plot(
        axis=zoomed_accuracy_axis,
        x_values=x_values,
        y_values=accuracies,
        lengths=lengths,
        y_label="Accuracy (%)",
        title=f"Accuracy vs Short Read Length (Without {time_focus_algorithm}, ver{version})",
        y_min=SETTINGS["accuracy_y_min"],
        y_max=SETTINGS["accuracy_y_max"],
        algorithm_names=non_focus_algorithms,
        x_offsets={
            "BWT": -0.12,
            "RabinKarp": 0.0,
            "IndexMapping": 0.12,
            "KMP": 0.24,
        },
    )

    create_scatter_plot(
        axis=focus_accuracy_axis,
        x_values=x_values,
        y_values=accuracies,
        lengths=lengths,
        y_label="Accuracy (%)",
        title=f"Accuracy for {time_focus_algorithm} (ver{version})",
        y_min=SETTINGS["accuracy_y_min"],
        y_max=SETTINGS["accuracy_y_max"],
        algorithm_names=[time_focus_algorithm],
    )

    time_figure.tight_layout()
    accuracy_figure.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()