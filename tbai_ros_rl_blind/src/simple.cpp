#include <iostream>
#include <memory>

#include "tbai_ros_rl/BobController.hpp"
#include <ros/ros.h>
#include <tbai_core/Logging.hpp>
#include <tbai_core/Utils.hpp>
#include <tbai_core/config/Config.hpp>
#include <tbai_core/control/CentralController.hpp>
#include <tbai_ros_core/Publishers.hpp>
#include <tbai_ros_core/Rate.hpp>
#include <tbai_ros_core/Subscribers.hpp>
#include <tbai_ros_reference/ReferenceVelocityGenerator.hpp>
#include <tbai_ros_static/StaticController.hpp>

int main(int argc, char *argv[]) {
    ros::init(argc, argv, "tbai_ros_static");
    ros::NodeHandle nh;

    // Set zero time
    tbai::writeInitTime(tbai::RosTime::rightNow());

    auto stateTopic = tbai::fromGlobalConfig<std::string>("state_topic");
    auto commandTopic = tbai::fromGlobalConfig<std::string>("command_topic");
    auto changeControllerTopic = tbai::fromGlobalConfig<std::string>("change_controller_topic");
    const std::string urdfString = nh.param<std::string>("robot_description", "");

    std::shared_ptr<tbai::StateSubscriber> stateSubscriber =
        std::shared_ptr<tbai::StateSubscriber>(new tbai::MuseRosStateSubscriber(nh, stateTopic, urdfString));

    std::shared_ptr<tbai::CommandPublisher> commandPublisher =
        std::shared_ptr<tbai::CommandPublisher>(new tbai::RosCommandPublisher(nh, commandTopic));

    std::shared_ptr<tbai::ChangeControllerSubscriber> changeControllerSubscriber =
        std::shared_ptr<tbai::ChangeControllerSubscriber>(
            new tbai::RosChangeControllerSubscriber(nh, changeControllerTopic));

    tbai::CentralController<ros::Rate, tbai::RosTime> controller(commandPublisher, changeControllerSubscriber);

    // Add static controller
    controller.addController(std::make_unique<tbai::static_::RosStaticController>(stateSubscriber));

    // Add Bob controller
    controller.addController(std::make_unique<tbai::rl::RosBobController>(
        urdfString, stateSubscriber, tbai::reference::getReferenceVelocityGeneratorShared(nh)));

    // Start controller loop
    controller.start();

    return EXIT_SUCCESS;
}
