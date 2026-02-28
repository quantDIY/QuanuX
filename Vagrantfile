Vagrant.configure("2") do |config|
  # Share the QuanuX root directory to /vagrant explicitly
  config.vm.synced_folder ".", "/vagrant"

  # Ubuntu 24.04 LTS (Current)
  config.vm.define "node_noble" do |noble|
    noble.vm.box = "bento/ubuntu-24.04"
    noble.vm.hostname = "quanux-node-noble"
    noble.vm.provider "virtualbox" do |vb|
      vb.memory = "2048"
      vb.cpus = 2
    end
    noble.vm.provision "shell", inline: "cd /vagrant && sudo bash QuanuX-Clustering/scripts/node_init.sh --tier 4"
  end

  # Ubuntu 22.04 LTS (Past 1)
  config.vm.define "node_jammy" do |jammy|
    jammy.vm.box = "ubuntu/jammy64"
    jammy.vm.hostname = "quanux-node-jammy"
    jammy.vm.provider "virtualbox" do |vb|
      vb.memory = "2048"
      vb.cpus = 2
    end
    jammy.vm.network "forwarded_port", guest: 4222, host: 4222
    jammy.vm.provision "shell", inline: "cd /vagrant && sudo bash QuanuX-Clustering/scripts/node_init.sh --tier 4"
  end

  # Ubuntu 20.04 LTS (Past 2)
  config.vm.define "node_focal" do |focal|
    focal.vm.box = "ubuntu/focal64"
    focal.vm.hostname = "quanux-node-focal"
    focal.vm.provider "virtualbox" do |vb|
      vb.memory = "2048"
      vb.cpus = 2
    end
    focal.vm.provision "shell", inline: "cd /vagrant && sudo bash QuanuX-Clustering/scripts/node_init.sh --tier 4"
  end
end
