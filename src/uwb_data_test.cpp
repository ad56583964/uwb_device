#include <ros/ros.h>
#include "std_msgs/String.h"

#include <iostream>
#include <boost/algorithm/string.hpp>

#include <uwb_device/Uwb_data.h>

//use the code from https://my.oschina.net/u/4339087/blog/3306241
int Hex_to_dec(std::string results)
{
  int temp;
  int sum = 0;      

  for(int i = 0; i <= 7; i++)
  {

    switch (results.c_str()[i])
    {
    case 'a': temp = 10; break;
    case 'b': temp = 11; break;
    case 'c': temp = 12; break;
    case 'd': temp = 13; break;
    case 'e': temp = 14; break;
    case 'f': temp = 15; break;
    default: temp = results.c_str()[i] - '0'; break;
    }
    sum = sum + temp * pow(16, 7-i);
  }

  return sum;
}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "uwb_data_test");
  
  ros::NodeHandle nh;

  ros::Publisher uwb_data_pub = nh.advertise<uwb_device::Uwb_data>("uwb_data", 1000);

  ros::Rate rate(50);
  //init the seriallib
  ROS_INFO("Start Change");
  

  char bufData[128] = "mc03178 4e 0004b45a a2:0\r\n";

  while(ros::ok())
  {
    ROS_INFO("???");
    if(bufData[0] == 'm' && bufData[61] == ':')
    {
        //Data print
        ROS_INFO("OKOK");
        ROS_INFO("%s",bufData);

        std::vector<std::string> results;

        boost::split(results, bufData, [](char c){return c == ' ';});

        
        uwb_device::Uwb_data pub_temp;

        pub_temp.a0 = Hex_to_dec(results[2]);

        uwb_data_pub.publish(pub_temp);

        ROS_INFO("a0: %d", pub_temp.a0);

        rate.sleep();
    }
  }

  return 0;
}
