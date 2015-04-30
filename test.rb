require 'orocos'


Orocos.run 'deployer::Task' => 'deployer' do

    d = Orocos::TaskContext.get 'deployer'
    d.configure

    d.deploy "message_producer::Task","dyn_deployed_producer1"
    d.deploy "mars::Task","dyn_mars"
    
    t1 = Orocos::TaskContext.get "dyn_deployed_producer1"
    mars = Orocos::TaskContext.get "dyn_mars"

    mars.configure
    mars.start

    t1.configure
    t1.start

    loop do
        sleep 1
    end
end
