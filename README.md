# Joy Watcher

`sensor_msgs/msg/Joy` の `joy` トピックをサブスクし、スティック、トリガー、全ボタンの値を表示します。

```bash
source /opt/ros/jazzy/setup.bash
colcon build --packages-select joy_watcher
source install/setup.bash
ros2 run joy_watcher joy_watcher_node
```


別途 `joy` トピックを配信するノードが必要です。
```bash
ros2 run joy joy_node
```


ここで使用している機器に設定を合わせてください。
```cpp
constexpr std::array<AxisDefinition, 6> kAxes{{
    {"left_stick_x", "Left stick X", 0},
    {"left_stick_y", "Left stick Y", 1},
    {"right_stick_x", "Right stick X", 3},
    {"right_stick_y", "Right stick Y", 2},
    {"left_trigger", "Left trigger", 4},
    {"right_trigger", "Right trigger", 5},
}};
```

起動引数でも設定可能です。
```bash
ros2 run joy_watcher joy_watcher_node --ros-args -p right_stick_x:=2 -p right_stick_y:=3 -p left_trigger:=4 -p right_trigger:=5
```

設定可能なパラメータ: `left_stick_x`, `left_stick_y`, `right_stick_x`, `right_stick_y`, `left_trigger`, `right_trigger`。
