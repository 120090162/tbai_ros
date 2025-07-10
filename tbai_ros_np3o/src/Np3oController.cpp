#include "tbai_ros_np3o/Np3oController.hpp"

namespace tbai {
namespace np3o {

RosNp3oController::RosNp3oController(const std::string &urdfString,
                                   const std::shared_ptr<tbai::StateSubscriber> &stateSubscriberPtr,
                                   const std::shared_ptr<tbai::reference::ReferenceVelocityGenerator> &refVelGenPtr)
    : tbai::Np3oController(urdfString, stateSubscriberPtr, refVelGenPtr) {
    timeSinceLastVisualizationUpdate_ = 1000.0;
}

void RosNp3oController::postStep(scalar_t currentTime, scalar_t dt) {
    if (timeSinceLastVisualizationUpdate_ >= 1.0 / 30.0) {
        auto state = getNp3oState();
        stateVisualizer_.visualize(state);
        contactVisualizer_.visualize(state_.x, state_.contactFlags);
        timeSinceLastVisualizationUpdate_ = 0.0;
    } else {
        timeSinceLastVisualizationUpdate_ += dt;
    }
}

void RosNp3oController::changeController(const std::string &controllerType, scalar_t currentTime) {
    preStep(currentTime, 0.0);
}


void RosNp3oController::stopController() {
    // Implementation for stopController
}

bool RosNp3oController::ok() const {
    return ros::ok();
}

void RosNp3oController::preStep(scalar_t currentTime, scalar_t dt) {
    ros::spinOnce();
    state_ = stateSubscriberPtr_->getLatestState();
}

}  // namespace np3o
}  // namespace tbai