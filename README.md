# Joy Watcher

`sensor_msgs/msg/Joy` の `joy` トピックを購読し、スティック、トリガー、全ボタンの値を表示します。軸は受信した値をそのまま表示します。ボタンは整数値を小数表示します。受信前や指定軸が存在しない場合は `N/A` です。

```bash
source /opt/ros/jazzy/setup.bash
colcon build --packages-select joy_watcher
source install/setup.bash
ros2 run joy_watcher joy_watcher_node
```

別途 `joy` トピックを配信するノードが必要です。トピック名は通常の ROS リマップで変更できます。

```bash
ros2 run joy_watcher joy_watcher_node --ros-args -r joy:=/controller/joy
```

軸の既定値は左スティック X/Y が 0/1、右スティック X/Y が 3/2、左/右トリガーが 4/5 です。機器やドライバーにより配置やトリガーの値の方向が異なるため、`ros2 topic echo /joy` などで軸の値を確認してパラメータを設定してください。

```bash
ros2 run joy_watcher joy_watcher_node --ros-args -p right_stick_x:=2 -p right_stick_y:=3 -p left_trigger:=4 -p right_trigger:=5
```

設定可能なパラメータ: `left_stick_x`, `left_stick_y`, `right_stick_x`, `right_stick_y`, `left_trigger`, `right_trigger`。
