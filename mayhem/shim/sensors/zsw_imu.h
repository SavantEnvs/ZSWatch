#pragma once
typedef enum zsw_imu_data_step_activity_t {
    ZSW_IMU_EVT_STEP_ACTIVITY_STILL,
    ZSW_IMU_EVT_STEP_ACTIVITY_WALK,
    ZSW_IMU_EVT_STEP_ACTIVITY_RUN,
    ZSW_IMU_EVT_STEP_ACTIVITY_UNKNOWN,
} zsw_imu_data_step_activity_t;
