#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Twist.h"

#include <sstream>
#include <string>

const char* gazebo_topic = "/cmd_vel_mux/input/teleop";
const double PI = std::atan(1.0)*4;

using namespace ros;

struct Point
{
    double x;
    double y;
    double z;

    Point( double _x, double _y, double _z): x(_x), y(_y), z(_z) {}
    const std::string toString() const
    {
        std::ostringstream str_stream;
        str_stream << "{ " << x << ", " << y << ", " << z << " }" << std::endl;

    }
};

//======================================================================================================================
void OdomCallback( const nav_msgs::Odometry::ConstPtr& msg )
{
    Point p(
    msg->pose.pose.position.x,
    msg->pose.pose.position.y,
    msg->pose.pose.position.z );
    ROS_INFO( "x: %f, y: %f, z: %f", p.x, p.y, p.z );
}

void WaitForClock()
{
    while (Time::now().toSec() <= 0)
    ;
}

//======================================================================================================================
void MoveForward( const Publisher &pub, const double max_speed, const double distance, const bool isForward=true )
{
    // for some reason the clock doesn't start for a few milliseconds, so give it some time
    WaitForClock();

    const double t0 = Time::now().toSec();
    double cur_distance = 0.0;

    geometry_msgs::Twist go_the_distance;

    go_the_distance.linear.x = ( isForward )? fabs( max_speed ) : -fabs( max_speed ); 
    go_the_distance.linear.y = 0.0;
    go_the_distance.linear.z = 0.0;
    go_the_distance.angular.x = 0.0;
    go_the_distance.angular.y = 0.0;
    go_the_distance.angular.z = 0.0;

    while ( cur_distance < distance )
    {
        pub.publish( go_the_distance );
        const double t1 = Time::now().toSec();
        cur_distance = max_speed * ( t1 - t0 );
    }

    go_the_distance.linear.x = 0.0;
    pub.publish( go_the_distance );
}

//======================================================================================================================
// note: parameters require angle units in radians
void Turn( const Publisher &pub, const double angular_speed_rad, const double relative_angle_final_deg, const bool clockwise )
{
    // for some reason the clock doesn't start for a few milliseconds, so give it some time
    WaitForClock();

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

//    auto tf::StampedTransform transform_in_map;
//    try
//    {
//        listener.lookupTransform("map", "base_footprint", ros::Time(0), transform_in_map);
//    }
//    catch ( tf::TransformException &e )
//    {
//        ROS_ERROR( "%s", e.what() );
//    }

    //For 2D turtle sim
    //Publisher pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
    //For turtle gazebo
    Publisher pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop", 10);
    Subscriber sub = nh.subscribe( "odom", 10, OdomCallback );
    if ( !pub )
    {
        ROS_FATAL("Publisher failed. Exiting now...");
    }

//    MoveForward( pub, 1, 2 );
//    Turn( pub, 1, 90, true );
//    MoveForward( pub, 1, 2, true );
//    Turn( pub, 1, 90, true );
//    MoveForward( pub, 1, 2, true );
//    Turn( pub, 1, 90, true );
//    MoveForward( pub, 1, 2, true );
//    Turn( pub, 1, 90, true );
    ros::spin();
    ROS_INFO("end");
}
