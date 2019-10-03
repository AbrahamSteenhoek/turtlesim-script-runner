#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Twist.h"

using namespace ros;

void OdomCallback( const nav_msgs::Odometry::ConstPtr& msg )
{
    double x = msg->pose.pose.position.x;
    double y = msg->pose.pose.position.y;
    ROS_INFO( "x: %f, y: %f", x, y );
}

int main ( int argc, char** argv )
{
    init( argc, argv, "turtlescript" );
    NodeHandle nh;
    //Subscriber sub = nh.subscribe( "odom" , 10, OdomCallback );
    Publisher pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop", 5);
    if ( !pub )
    {
        ROS_FATAL("Publisher failed. Exiting now...");
        return -1;
    }
    geometry_msgs::Twist forward;
    forward.linear.x = 5;
    while(true)
        pub.publish(forward);

    ROS_INFO("Shutting down teleop topic");
    //pub.shutdown();
    spin();
}