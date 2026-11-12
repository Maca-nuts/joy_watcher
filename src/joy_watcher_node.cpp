#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include <QApplication>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"

namespace {
constexpr int kUpdateIntervalMs = 20;
constexpr int kDecimalPlaces = 3;

struct AxisDefinition {
  const char *parameter_name;
  const char *caption;
  int default_index;
};

constexpr std::array<AxisDefinition, 6> kAxes{{
    {"left_stick_x", "Left stick X", 0},
    {"left_stick_y", "Left stick Y", 1},
    {"right_stick_x", "Right stick X", 3},
    {"right_stick_y", "Right stick Y", 2},
    {"left_trigger", "Left trigger", 4},
    {"right_trigger", "Right trigger", 5},
}};

struct AxisDisplay {
  int index;
  QLabel *value;
};
} // namespace

class JoyWatcher : public QWidget {
public:
  JoyWatcher() : node_(std::make_shared<rclcpp::Node>("joy_watcher")) {
    setupWindow();

    subscription_ = node_->create_subscription<sensor_msgs::msg::Joy>(
        "joy", rclcpp::SensorDataQoS(),
        [this](sensor_msgs::msg::Joy::ConstSharedPtr message) {
          updateDisplay(*message);
        });

    // Process ROS callbacks on the Qt thread so widgets are only updated there.
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
      if (rclcpp::ok()) {
        rclcpp::spin_some(node_);
      } else {
        QApplication::quit();
      }
    });
    timer->start(kUpdateIntervalMs);
  }

private:
  void setupWindow() {
    auto *layout = new QVBoxLayout(this);
    status_ = new QLabel("Waiting for Joy messages on joy", this);
    layout->addWidget(status_);
    layout->addWidget(createAxisGroup());
    layout->addWidget(
        new QLabel("All buttons (0.0 = released, 1.0 = pressed)", this));
    buttons_table_ = createButtonsTable();
    layout->addWidget(buttons_table_);

    setWindowTitle("Joy Watcher");
    resize(480, 700);
  }

  QGroupBox *createAxisGroup() {
    auto *group = new QGroupBox("Sticks and triggers (raw axes)", this);
    auto *form = new QFormLayout(group);
    for (const auto &axis : kAxes) {
      const int index = node_->declare_parameter<int>(axis.parameter_name,
                                                      axis.default_index);
      auto *value = new QLabel("N/A", group);
      axis_displays_.push_back({index, value});
      form->addRow(QString("%1 (axis %2)").arg(axis.caption).arg(index), value);
    }
    return group;
  }

  QTableWidget *createButtonsTable() {
    auto *table = new QTableWidget(0, 2, this);
    table->setHorizontalHeaderLabels({"Button", "Value"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->hide();
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    return table;
  }

  void updateDisplay(const sensor_msgs::msg::Joy &message) {
    status_->setText("Receiving Joy messages");
    for (const auto &axis : axis_displays_) {
      if (axis.index < 0 ||
          static_cast<std::size_t>(axis.index) >= message.axes.size()) {
        axis.value->setText("N/A");
      } else {
        const auto value = message.axes[static_cast<std::size_t>(axis.index)];
        axis.value->setText(QString::number(value, 'f', kDecimalPlaces));
      }
    }
    updateButtons(message.buttons);
  }

  void updateButtons(const std::vector<int32_t> &buttons) {
    buttons_table_->setRowCount(static_cast<int>(buttons.size()));
    for (std::size_t i = 0; i < buttons.size(); ++i) {
      const int row = static_cast<int>(i);
      if (!buttons_table_->item(row, 0)) {
        buttons_table_->setItem(row, 0,
                                new QTableWidgetItem(QString::number(row)));
        buttons_table_->setItem(row, 1, new QTableWidgetItem());
      }
      buttons_table_->item(row, 1)->setText(QString::number(
          static_cast<double>(buttons[i]), 'f', kDecimalPlaces));
    }
  }

  rclcpp::Node::SharedPtr node_;
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_;
  std::vector<AxisDisplay> axis_displays_;
  QLabel *status_ = nullptr;
  QTableWidget *buttons_table_ = nullptr;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  // ROS arguments are handled by rclcpp; pass only the program name to Qt.
  int qt_argc = 1;
  QApplication app(qt_argc, argv);
  JoyWatcher window;
  window.show();
  const int result = app.exec();
  rclcpp::shutdown();
  return result;
}
