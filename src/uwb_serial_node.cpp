#include <ros/ros.h>
#include "std_msgs/String.h"
#include "serialib.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

#include <uwb_device/Uwb_data.h>

std::vector<std::string> results;

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
  ros::init(argc, argv, "uwb_serial");
  
  ros::NodeHandle nh;

  ros::Publisher uwb_data_pub = nh.advertise<uwb_device::Uwb_data>("uwb_data", 1000);


  ros::Rate rate(50);

  //init the seriallib
  serialib serial;
  while (serial.openDevice("/dev/ttyACM0", 115200) !=1 )
  {
    ROS_INFO("Serial_Failed");
  }

    ROS_INFO("Serial_Success");


  char bufData[256];
  char mask;

  while(ros::ok())
  {
        //blocked?? uart receive

    int length = serial.readString(bufData,'\n',256,15);

    if(length == 65 && bufData[0] == 'm' && bufData[61] == ':')
    {
      
        //Data print
        ROS_INFO("Data");
        ROS_INFO("%s", bufData);

        boost::split(results, bufData, [](char c){return c == ' ';});

        uwb_device::Uwb_data pub_temp;

        pub_temp.header.stamp = ros::Time::now();;
        pub_temp.a0 = Hex_to_dec(results[2]);
        pub_temp.a1 = Hex_to_dec(results[3]);
        pub_temp.a2 = Hex_to_dec(results[4]);
        pub_temp.a3 = Hex_to_dec(results[5]);
        
        uwb_data_pub.publish(pub_temp);
        ROS_INFO("a0: %d,a1: %d,a2: %d,a3: %d", pub_temp.a0,pub_temp.a1,pub_temp.a2,pub_temp.a3);

    }
    else
    {
      ROS_INFO("Data Error");
    }


    rate.sleep();
  }
  return 0;
}
