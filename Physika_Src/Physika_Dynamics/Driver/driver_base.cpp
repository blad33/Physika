/*
 * @file driver_base.cpp 
 * @brief Base class of all driver classes. A driver class manages the simulation process.
 * @author Fei Zhu
 * 
 * This file is part of Physika, a versatile physics simulation library.
 * Copyright (C) 2013 Physika Group.
 *
 * This Source Code Form is subject to the terms of the GNU General Public License v2.0. 
 * If a copy of the GPL was not distributed with this file, you can obtain one at:
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#include <iostream>
#include "Physika_Dynamics/Driver/driver_base.h"
#include "Physika_Dynamics/Driver/driver_plugin_base.h"

namespace Physika{

template <typename Scalar>
DriverBase<Scalar>::DriverBase()
    :start_frame_(0),end_frame_(0),restart_frame_(0),frame_rate_(0),
    max_dt_(0),write_to_file_(false),enable_timer_(true),
    time_(0)
{
}

template <typename Scalar>
DriverBase<Scalar>::DriverBase(unsigned int start_frame, unsigned int end_frame, Scalar frame_rate, Scalar max_dt, bool write_to_file)
    :start_frame_(start_frame),end_frame_(end_frame),restart_frame_(0),frame_rate_(frame_rate),
    max_dt_(max_dt),write_to_file_(write_to_file),enable_timer_(true),
    time_(0)
{
}


template <typename Scalar>
DriverBase<Scalar>::~DriverBase()
{
}

template <typename Scalar>
void DriverBase<Scalar>::run()
{
    initialize();

    for(unsigned int frame=start_frame_;frame<=end_frame_;++frame)
    {
        std::cout<<"Begin Frame "<<frame<<"\n";

        unsigned int plugin_num = static_cast<unsigned int>(plugins_.size());
        DriverPluginBase<Scalar>* plugin;
        for(unsigned int i = 0; i < plugin_num; ++i)
        {
            plugin = plugins_[i];
            if(plugin != NULL)
                plugin->onBeginFrame(frame);
        }

        if(enable_timer_) timer_.startTimer();
        advanceFrame();
        std::cout<<"End Frame "<<frame<<" ";
        if(enable_timer_)
        {
            timer_.stopTimer();
            std::cout<<timer_.getElapsedTime()<<" s";
        }
        std::cout<<"\n";

        for(unsigned int i = 0; i < plugin_num; ++i)
        {
            plugin = plugins_[i];
            if(plugin != NULL)
                plugin->onEndFrame(frame);
        }

        if(write_to_file_)
        {
            std::string file_name="Frame "+frame;
            write(file_name.c_str());
        }
    }
}

template <typename Scalar>
void DriverBase<Scalar>::advanceFrame()
{
    Scalar frame_dt=1.0/frame_rate_;
    Scalar initial_time=time_;
    Scalar finish_time=time_+frame_dt;
    bool frame_done=false;
    while(!frame_done)
    {
        //begin time step callbacks
        unsigned int plugin_num = static_cast<unsigned int>(plugins_.size());
        DriverPluginBase<Scalar>* plugin;
        for(unsigned int i = 0; i < plugin_num; ++i)
        {
            plugin = plugins_[i];
            if(plugin != NULL)
                plugin->onBeginTimeStep(time_);
        }

        //compute time step
        Scalar dt=computeTimeStep();
        dt=(dt>max_dt_)?max_dt_:dt;
        //update time and maybe time step
        if(finish_time-time_<=dt)
        {
            dt=finish_time-time_;
            time_=finish_time;
            frame_done=true;
        }
        else
            time_+=dt;
        //advance step
        advanceStep(dt);

        //end time step callbacks
        for(unsigned int i = 0; i < plugin_num; ++i)
        {
            plugin = plugins_[i];
            if(plugin != NULL)
                plugin->onEndTimeStep(time_, dt);
        }
    }

}

template <typename Scalar>
void DriverBase<Scalar>::addPlugin(DriverPluginBase<Scalar>* plugin)
{
    if(plugin == NULL)
    {
        std::cerr<<"Cannot add NULL plugin, operation ignored!"<<std::endl;
        return;
    }
    plugin->setDriver(this);
    plugins_.push_back(plugin);
}

//explicit instantiation
template class DriverBase<float>;
template class DriverBase<double>;

}  //end of namespace Physika
