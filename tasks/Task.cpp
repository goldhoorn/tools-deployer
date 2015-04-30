/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <orogen/deployer.hpp>
#include <rtt/transports/corba/TaskContextServer.hpp>
#include <rtt/transports/corba/CorbaDispatcher.hpp>


using namespace deployer;

Task::Task(std::string const& name)
    : TaskBase(name),
    loader(false)
{
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine)
    : TaskBase(name, engine),
    loader(false)
{
}

Task::~Task()
{
}

bool Task::deploy(::std::string const & task_name, ::std::string const & deployed_name)
{

    std::cerr << "Deploy " << task_name << std::endl;


    boost::shared_ptr<orogen::Deployer> task;
    try{
        task = loader.createInstance<orogen::Deployer>(task_name + "Deployer");
    }catch(...){
        LOG_ERROR_S << "Cannot load task spec for " << task_name << std::endl;
        return false;
    }

    boost::shared_ptr< ::RTT::TaskContext > ptr( task->getTask(deployed_name)); 
    createdTasks.push_back(ptr); 
    
    RTT::corba::TaskContextServer::Create( ptr.get() );
    RTT::corba::CorbaDispatcher::Instance( ptr->ports(), ORO_SCHED_OTHER, RTT::os::LowestPriority );

    //Proof of concept implementation, create only periodic activity

    RTT::Activity* act = new RTT::Activity(
    ORO_SCHED_OTHER,
    RTT::os::LowestPriority,
    1.0,
    ptr->engine(),
    deployed_name);

    RTT::os::Thread* thr= dynamic_cast<RTT::os::Thread*>(act->thread());
    thr->setMaxOverrun(-1);

    
    { RTT::os::Thread* thread = dynamic_cast<RTT::os::Thread*>(act);
        if (thread)
            thread->setStopTimeout(10);
    }
    
    std::cerr << "Deploy successful" << std::endl;

    return true;
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

bool Task::configureHook()
{
    if (! TaskBase::configureHook())
        return false;
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(_libdir.get().c_str())) == NULL) {
        LOG_FATAL_S << "Cannot open dir " << _libdir.get() << std::endl;
        return false;
    }
    while ((dirp = readdir(dp)) != NULL) {
        if(std::string(dirp->d_name).find(".so") != std::string::npos){
            LOG_DEBUG_S << "Loading lib: " <<  dirp->d_name << std::endl;
            try{
            loader.loadLibrary(_libdir.get() + std::string("/") + std::string(dirp->d_name));
            }catch(...){
                LOG_ERROR_S << "Error occured during loading of " << _libdir.get() + std::string("/") + std::string(dirp->d_name) << std::endl;
            }
        }
    }
    std::vector<std::string> classes = loader.getAvailableClasses< ::orogen::Deployer >();
    std::stringstream str("Known TaskContext: ");
    for(auto i : classes){
        str << i << " ";
    }
    LOG_DEBUG_S << str.str() << std::endl;
    return true;
}
bool Task::startHook()
{
    if (! TaskBase::startHook())
        return false;
    return true;
}
void Task::updateHook()
{
    TaskBase::updateHook();
}
void Task::errorHook()
{
    TaskBase::errorHook();
}
void Task::stopHook()
{
    TaskBase::stopHook();
}
void Task::cleanupHook()
{
    TaskBase::cleanupHook();
}

