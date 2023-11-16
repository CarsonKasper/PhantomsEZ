#include "main.h"

using namespace pros;

Controller master(E_CONTROLLER_MASTER);
Motor rightCatapult(10, E_MOTOR_GEAR_RED, true);
Motor leftCatapult(9, E_MOTOR_GEAR_RED);
Motor_Group cataGroup({rightCatapult, leftCatapult});
Rotation rotationSensor(2,true);
ADIDigitalOut SolenoidR('A');
ADIDigitalOut SolenoidL('B');

/////
// For instalattion, upgrading, documentations and tutorials, check out website!
// https://ez-robotics.github.io/EZ-Template/
/////


// Chassis constructor
Drive chassis (
  // Left Chassis Ports (negative port will reverse it!)
  //   the first port is the sensored port (when trackers are not used!)
  {-6, -4, -8}

  // Right Chassis Ports (negative port will reverse it!)
  //   the first port is the sensored port (when trackers are not used!)
  ,{5, 3, 7}

  // IMU Port
  ,21

  // Wheel Diameter (Remember, 4" wheels are actually 4.125!)
  //    (or tracking wheel diameter)
  ,3.25

  //   tick per rotation for tracking wheels)
  ,360

  // gear ratio of tracking wheel)
  ,1.66666667

  // Uncomment if using tracking wheels
  
  // Left Tracking Wheel Ports (negative port will reverse it!)
  // ,{1, 2} // 3 wire encoder
  ,1 // Rotation sensor

  // Right Tracking Wheel Ports (negative port will reverse it!)
  // ,{-3, -4} // 3 wire encoder
  // ,-9 // Rotation sensor
  

  // Uncomment if tracking wheels are plugged into a 3 wire expander
  // 3 Wire Port Expander Smart Port
  // ,1
);



/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  // Print our branding over your terminal :D
  ez::print_ez_template();
  
  pros::delay(500); // Stop the user from doing anything while legacy ports configure.

  // Configure your chassis controls
  chassis.toggle_modify_curve_with_controller(true); // Enables modifying the controller curve with buttons on the joysticks
  chassis.set_active_brake(0.1); // Sets the active brake kP. We recommend 0.1.
  chassis.set_curve_default(1, 1); // Defaults for curve. If using tank, only the first parameter is used. (Comment this line out if you have an SD card!)  
  default_constants(); // Set the drive to your own constants from autons.cpp!
  exit_condition_defaults(); // Set the exit conditions to your own constants from autons.cpp!

  // These are already defaulted to these buttons, but you can change the left/right curve buttons here!
  // chassis.set_left_curve_buttons (pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT); // If using tank, only the left side is used. 
  // chassis.set_right_curve_buttons(pros::E_CONTROLLER_DIGITAL_Y,    pros::E_CONTROLLER_DIGITAL_A);

  // Autonomous Selector using LLEMU
  ez::as::auton_selector.add_autons({
    Auton("Offensive", offensive),
    Auton("Defensive", defensive),
    Auton("Skills", skills),
    // Auton("Example Drive\n\nDrive forward and come back.", ),
    // Auton("Example Drive\n\nDrive forward and come back.", drive_example),
    // Auton("Example Turn\n\nTurn 3 times.", turn_example),
    // Auton("Drive and Turn\n\nDrive forward, turn, come back. ", drive_and_turn),
    // Auton("Drive and Turn\n\nSlow down during drive.", wait_until_change_speed),
    // Auton("Swing Example\n\nSwing, drive, swing.", swing_example),
    // Auton("Combine all 3 movements", combining_movements),
    // Auton("Interference\n\nAfter driving forward, robot performs differently if interfered or not.", interfered_example),
  });

  // Initialize chassis and auton selector
  chassis.initialize();
  ez::as::initialize();

  SolenoidL.set_value(LOW);
	SolenoidR.set_value(HIGH);
	cataGroup.set_brake_modes(MOTOR_BRAKE_HOLD);
	cataGroup.set_encoder_units(MOTOR_ENCODER_DEGREES);
}

void rotationalCata() {
	while (true)
	{
		while ((rotationSensor.get_angle() < 7700) || (rotationSensor.get_angle() > 8500))
		{
			cataGroup.move(127);
		}
		cataGroup.move(0);
		cataGroup.brake();
		if (master.get_digital(E_CONTROLLER_DIGITAL_R2))
		{
			int motorP = rightCatapult.get_position();
			int fireM = 20;
			int grace = 5;
			cataGroup.move_relative(fireM, 100);
			while ((rightCatapult.get_position() < motorP + fireM - grace) || (rightCatapult.get_position() > motorP + fireM + grace)){
				delay(5);
			}
			delay(100);
		}	
	}
}

void wingControl(){
	int OCINT = 0;
	while (true)
	{
		if (master.get_digital(E_CONTROLLER_DIGITAL_L2) == 1)
		{
			if(OCINT == 0)
			{
				SolenoidL.set_value(HIGH);
				SolenoidR.set_value(LOW);
				OCINT = 1;
				delay(500);
			}
			else
			{
				SolenoidL.set_value(LOW);
				SolenoidR.set_value(HIGH);
				OCINT = 0;
				delay(500);
			}
			
		}
		if (master.get_digital(E_CONTROLLER_DIGITAL_L1) == 1)
		{
			if(OCINT == 0)
			{
				SolenoidL.set_value(HIGH);
				OCINT = 1;
				delay(500);
			}
			else
			{
				SolenoidL.set_value(LOW);
				OCINT = 0;
				delay(500);
			}
		}
		if (master.get_digital(E_CONTROLLER_DIGITAL_R1) == 1)
		{
			if(OCINT == 0)
			{
				SolenoidR.set_value(LOW);
				OCINT = 1;
				delay(500);
			}
			else
			{
				SolenoidR.set_value(HIGH);
				OCINT = 0;
				delay(500);
			}
			
		}
	}
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  // . . .
}



/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
  // . . .
}



/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  chassis.reset_pid_targets(); // Resets PID targets to 0
  chassis.reset_gyro(); // Reset gyro position to 0
  chassis.reset_drive_sensor(); // Reset drive sensors to 0
  chassis.set_drive_brake(MOTOR_BRAKE_HOLD); // Set motors to hold.  This helps autonomous consistency.

  ez::as::auton_selector.call_selected_auton(); // Calls selected auton from autonomous selector.
}



/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
  // This is preference to what you like to drive on.
  chassis.set_drive_brake(MOTOR_BRAKE_COAST);
  SolenoidL.set_value(LOW);
	SolenoidR.set_value(HIGH);
	Task fireCatapult(rotationalCata);
	Task MannageWings(wingControl);
  while (true) {

    // chassis.tank(); // Tank control
    chassis.arcade_standard(ez::SPLIT); // Standard split arcade
    // chassis.arcade_standard(ez::SINGLE); // Standard single arcade
    // chassis.arcade_flipped(ez::SPLIT); // Flipped split arcade
    // chassis.arcade_flipped(ez::SINGLE); // Flipped single arcade



    pros::delay(ez::util::DELAY_TIME); // This is used for timer calculations!  Keep this ez::util::DELAY_TIME
  }
}
