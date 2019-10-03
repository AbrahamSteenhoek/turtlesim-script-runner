#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Twist.h"

const char* gazebo_topic = "/cmd_vel_mux/input/teleop";

using namespace ros;

void OdomCallback( const nav_msgs::Odometry::ConstPtr& msg )
{
    double x = msg->pose.pose.position.x;
    double y = msg->pose.pose.position.y;
    ROS_INFO( "x: %f, y: %f", x, y );
}

void MoveForward( const Publisher &pub, const double speed, const double distance, const bool isForward=true )
{

    while (Time::now().toSec() <= 0)
    ;

    const double t0 = Time::now().toSec();
    ROS_INFO( "t0 = %f", t0 );
    double cur_distance = 0;

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

    MoveForward( pub, 2, atof( argv[1] ) );
    ROS_INFO("end");
}