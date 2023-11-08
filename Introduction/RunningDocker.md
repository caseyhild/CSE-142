# Ways To Run Docker

All your work for the class will be done in a docker container. Docker containers are self-contained Linux-based workspaces.  Using docker ensures that your code runs in a consistent environment for you and the autograder/course staff.

The officially supported way to run that container is by logging into campus' `dslmp` system.

However, you can also run the container on your own machine.  To use the Moneta tool, you'll need > 8GB of memory available for the docker container.  In practice, this means you'll need 12 or 16GB of DRAM on your machine.

Some parts of some labs don't need Moneta.  For those, you'll only need 2GB.

The docker image is also large (~12GB), so you'll need diskspace to hold that.

Running on your own hardware is not supported by the course staff.  If you can't get it to work, use `dslmp`.

## Run Docker Locally on Your Mac

Start by following the instructions here to install docker:
https://docs.docker.com/install/.  This seems to work pretty easily, but the course staff is not able to assist in getting docker running on your Mac.

Once that's done, you should be able to run `docker` in the terminal (see [here](#start-a-docker-container) to learn how
to run the container)

## Run Docker on Your Windows Machine

It is possible to run Docker on Windows, but we've had many issues with, and we don't support it.  The lab staff is not able to assist with getting docker running under windows.


[Here](https://hub.docker.com/editions/community/docker-ce-desktop-windows) is a
short tutorial to help you install Docker on Windows 10.

1.  Install Windows 10 Pro or Education
    - If you have Windows 10 Pro or Education, continue to step 2. If you do not have Windows 10 Pro or Education, follow the steps below.
    - Follow the link below to get a free license for Windows 10 Education. You will have to make an account in order to get the free code. You must use your UCSD email.
[https://ucsd.onthehub.com/WebStore/OfferingDetails.aspx?o=9b3bfdc0-71e9-e611-9425-b8ca3a5db7a1](https://ucsd.onthehub.com/WebStore/OfferingDetails.aspx?o=9b3bfdc0-71e9-e611-9425-b8ca3a5db7a1)
    - Go to your windows 10 settings -> Update & Security -> Activation -> Change Product Key. Copy and paste your free key and install Windows
2.  Install Docker Desktop
    - Follow this link to download the docker desktop installer. You will need to create an account. Please do not install docker toolbox.
[https://hub.docker.com/?overlay=onboarding](https://hub.docker.com/?overlay=onboarding)
    - Run the installer and if it asks to enable hyper-v after it installs say yes. Your computer may need to restart.
    - Upon restarting, run docker desktop. You’ll know docker has started when the icon in your system tray stops animating.
    - Right click the docker icon and go to settings -> shared drives and click the checkbox next to the drive you wish to keep your labs on. You will need to enter your account password.
3.  Test your install
Open powershell and run the command:
```
docker run hello-world
```
Your output should look like this. (You may have extra text pulling the hello-world image)
```
Hello from Docker!
This message shows that your installation appears to be working correctly.

To generate this message, Docker took the following steps:
 1. The Docker client contacted the Docker daemon.
 2. The Docker daemon pulled the "hello-world" image from the Docker Hub.
    (amd64)
 3. The Docker daemon created a new container from that image which runs the
    executable that produces the output you are currently reading.
 4. The Docker daemon streamed that output to the Docker client, which sent it
    to your terminal.

To try something more ambitious, you can run an Ubuntu container with:
 $ docker run -it ubuntu bash

Share images, automate workflows, and more with a free Docker ID:
 https://hub.docker.com/

For more examples and ideas, visit:
 https://docs.docker.com/get-started/
```
4. To start docker into the development environment for the labs
Run the command replacing the tagged parts  (see [here](#start-a-docker-container) for more details):
```
docker run -it --volume <Directory to your lab folder>:/runner/<Your lab folder name> ucsdnvsl/cse141pp:latest
```


Change directories to land where your files should be.
```
cd ../runner
```

## Use VirtualBox to Run Linux (on Mac or Windows if you have Intel VT-X or AMD-V support)

1.  Download and install VirtualBox: https://www.virtualbox.org/wiki/Downloads
2.  Open VirtualBox
3.  Click "New"
4.  Set "Type" = "Linux" and "Version" = "Ubuntu (64-bit)".
5.  The VM will need at least a couple GB of memory.
6.  Click through the default to create a disk.  You'll need at least 30GB.
7.  Download this: http://releases.ubuntu.com/16.04/ubuntu-16.04.6-desktop-amd64.iso 
8.  Go to `Settings->Storage`, click the '+' disk icon to add an optical drive.  Then `Add` and select the iso image above.  Click 'ok'.
9.  Click 'start' -- your VM should boot.
10. Click 'Install Ubuntu' and accept all the defaults.
    - If you are unable to see the buttons due to the screen being too small, you can press and hold *Alt* in windows then click the mouse to move the screen around.
11. Restart the VM when asked.
12. When it comes back, right click->Terminal.
13. Install docker with `sudo apt-get install docker.io`.
14. Follow these instructions to install the "Guest Additions": https://www.virtualbox.org/manual/ch04.html#additional-linux-install.  This will make working with the VM easier.
   

Type `docker --help ` at the command line.  It should printout some usage information:

```
Usage:    docker [OPTIONS] COMMAND

A self-sufficient runtime for containers

Options:
      --config string      Location of client config files (default "/home/swanson/.docker")
      -D, --debug              Enable debug mode
      -H, --host list          Daemon socket(s) to connect to
...
```

## Use Docker for Windows (on Windows if you don't have Intel VT-X or AMD-V support)

Follow directions here: https://docs.docker.com/docker-for-windows/

## Start a Docker Container

This only applies if you are running docker on your own machine (i.e., not on `dsmlp-login`).  If you are running on `dsmlp-login`, then you should skip this.

The docker image you'll be using is `ucsdnvsl/CSE141pp:latest`.


You can start it from the command line with 

```
docker run -it --publish published=8888,target=8888 --mount type=bind,source=$HOME,dst=/root  --env HOME=/root --workdir /cse142L  --env THUNK_TO_HOME=yes stevenjswanson/cse142l-swanson-runner:latest bash --rcfile env.sh
```

It does a few things:

1. It exports port 8888, which is where Jupyter notebook will run.
2. It It mounts your home directory at `/root`.  This makes your files
   available inside the container (so you can edit them outside and see the
   changes inside) and also makes your ssh keys work.
3. It sets the `HOME` environment variable.
4. `--workdir /cse142L  --env THUNK_TO_HOME=yes` is some magic to make sure everything gets configured right.
5. It starts bash and has it set some environment variables in the `env.sh`

Then it starts, it'll will try to create an ssh agent so you don't have to type
your password over and over.  If your ssh private key is password protected,
it'll ask you for it.

You'll be dropped into `/root` which will contain the contents of your home
directory on the host machine.  Changes made inside the docker container will
be visible out side and vice-versa.  So that I can do the following (the
`747f4ec8585b:` prompt means I'm inside the container):

```
Kilgores-MacBook-Pro-1323:~ ls *hello*
ls: *hello*: No such file or directory
Kilgores-MacBook-Pro-1323:~ docker run -it --publish published=8888,target=8888 --mount type=bind,source=$HOME,dst=/root  --env HOME=/root --workdir /cse142L  --env THUNK_TO_HOME=yes stevenjswanson/cse142l-swanson-runner:latest bash --rcfile env.sh
DJR_CLUSTER=djr-default-cluster
THIS_DOCKER_IMAGE=stevenjswanson/cse142l-swanson-runner:v3
THIS_DOCKER_CONTAINER=
THIS_DOCKER_IMAGE_UUID=42e0286a-adcb-415e-b40c-c304900e0936
WORK_OUTSIDE_OF_DOCKER=
REAL_IP_ADDR=
Agent pid 38
Enter passphrase for /root/.ssh/id_rsa:
Identity added: /root/.ssh/id_rsa (/root/.ssh/id_rsa)
Identity added: /root/.ssh/id_dsa (/root/.ssh/id_dsa)
747f4ec8585b:~ $ ls *hello*
ls: cannot access '*hello*': No such file or directory
747f4ec8585b:~ $ touch hello
747f4ec8585b:~ $ exit
exit
Kilgores-MacBook-Pro-1323:~ ls *hello*
hello
```

Note that I can see the contents of my home directory from inside
docker, and changes I make inside remain after I exit.  

You may need to tweak the command to suit you needs.

## Testing Your Docker Container


To make sure everything is working, you can run `cse142 --help`.  You should get something like this:

```
Usage: cse142 [OPTIONS] COMMAND [ARGS]...

Options:
  --server URL        Base url for the HTTP interface to the cluster.
  --http / --no-http  Use HTTP or connect directly
  -v, --verbose       Be Verbose
  -d, --debug         Enable debugging
  --json              Output results in json
  --trace             Drop into the debugger immediately
  --pdb               Invoke the debugger on uncaught exception.
  -n, --dry-run       Don't do anything, just say what would be done.
  --job-type TEXT     class name of job type to use
  --help              Show this message and exit.

Commands:
  cluster
  dev      Run a docker image configured for use with cse142L.
  job
  lab
  login
  logout
  user
```

## Getting Access to Jupyter Notebook

To start Jupyter Notebook, run this from inside docker in your home directory:

``` 
jupyter-notebook --allow-root .
```

