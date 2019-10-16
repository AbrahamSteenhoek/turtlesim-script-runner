#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Twist.h"

const char* gazebo_topic = "/cmd_vel_mux/input/teleop";
const double PI = std::atan(1.0)*4;

using namespace ros;

//======================================================================================================================
void OdomCallback( const nav_msgs::Odometry::ConstPtr& msg )
{
    double x = msg->pose.pose.position.x;
    double y = msg->pose.pose.position.y;
    ROS_INFO( "x: %f, y: %f", x, y );
}

//======================================================================================================================
void MoveForward( const Publisher &pub, const double speed, const double distance, const bool isForward=true )
{
    // for some reason the clock doesn't start for a few milliseconds, so give it some time
    while (Time::now().toSec() <= 0)
    ;

    const double t0 = Time::now().toSec();
    double cur_distance = 0.0;

    geometry_msgs::Twist go_the_distance;

    go_the_distance.linear.x = ( isForward )? fabs( speed ) : -fabs( speed ); 
    go_the_distance.linear.y = 0.0;
    go_the_distance.linear.z = 0.0;
    go_the_distance.angular.x = 0.0;
    go_the_distance.angular.y = 0.0;
    go_the_distance.angular.z = 0.0;

    while ( cur_distance < distance )
    {
        pub.publish( go_the_distance );
        const double t1 = Time::now().toSec();
        cur_distance = speed * ( t1 - t0 );
    }

    go_the_distance.linear.x = 0.0;
    pub.publish( go_the_distance );
}

//======================================================================================================================
// note: parameters require angle units in radians
void Turn( const Publisher &pub, const double angular_speed_rad, const double relative_angle_final_deg, const bool clockwise )
{
    // for some reason the clock doesn't start for a few milliseconds, so give it some time
    while (Time::now().toSec() <= 0)
    ;
    //const double angular_speed_rad = angular_speed_deg * 2 * PI / 360;
    const double relative_angle_final_rad = relative_angle_final_deg * 2 * PI / 360;

    const double t0 = Time::now().toSec();
    double cur_angle = 0.0;

    geometry_msgs::Twist turny_turn;

    turny_turn.linear.x = 0.0;
    turny_turn.linear.y = 0.0;
    turny_turn.linear.z = 0.0;
    turny_turn.angular.x = 0.0;
    turny_turn.angular.y = 0.0;

    turny_turn.angular.z = ( clockwise )? -fabs( angular_speed_rad ) : fabs( angular_speed_rad );

    while ( cur_angle < relative_angle_final_rad )
    {
        pub.publish( turny_turn );
        const double t1 = Time::now().toSec();
        cur_angle = angular_speed_rad * ( t1 - t0 );
    }

    turny_turn.angular.z = 0.0;
    pub.publish( turny_turn );
    ROS_INFO("done turning!");
}

int main ( int argc, char** argv )
{
    init( argc, argv, "turtlescript" );
    ROS_INFO("start");
    NodeHandle nh;

    //For 2D turtle sim
    //Publisher pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
    //For turtle gazebo
    Publisher pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop", 5);
    if ( !pub )
    {
        ROS_FATAL("Publisher failed. Exiting now...");
    }

    MoveForward( pub, 1, 2 );
    Turn( pub, 1, 90, true );
    MoveForward( pub, 1, 2, true );

    ROS_INFO("end");
}
