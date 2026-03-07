---
marp: true
size: 16:9
style: |
    img[alt~="center"] {
      display: block;
      margin: 0 auto;
    }
---

# Laboratory 02
## Remote Development Environment

### Paolo Joseph Baioni
[paolojoseph.baioni@polimi.it](mailto:paolojoseph.baioni@polimi.it)
### 06/03/2026

---
## Outline
1. Setting up the VPN and SSH connection
2. Useful bash commands
3. Users, groups and permissions
4. File System Hierarchy
5. Network File System
6. Scratch
7. Open Portable Batch System
8. Spack and containers
9. Exercise: pacs-labs (HPC cluster workflow)
10. Q&A

---
## 1. Setting up the VPN and SSH connection

The first step to access the cluster is to connect to the VPN.

You should follow the [PoliMi official instructions](https://www.ict.polimi.it/network/vpn/) and select the guidelines for the Department of Mathematics.

In particular, Linux users may refer to the additional guide available on WeBeep.

Note that the VPN may block access to some PoliMi websites; you won’t be able to use WeBeep and the VPN at the same time.

---
## 1. Setting up the VPN and SSH connection

Once the VPN connection is active, you should be able to:

```bash
ping -c 5 IP
````

where `IP` is the address of the cluster login node (see the WeBeep cluster guide).

If the `ping` command is successful (i.e. 0% packet loss), you can proceed with the login.

From the terminal where you generated your SSH keys, simply run:

```bash
ssh username@IP
```

where `username` is `u<person_code>`, e.g. `u12345678`.

---

### Troubleshooting

`ssh` accepts various command-line options that can be useful, in particular:

* if you have generated keys in non-standard paths, you can specify the private key using `-i`;
* in case of errors, you can enable verbose mode with `-vv`.

---

## 2. Useful bash commands

The following commands from Lab_01 are required to follow the rest of the laboratory:

```bash
ls     # lists contents
nano   # opens a simple terminal-based text editor
pwd    # prints current directory
cd     # changes directory
whoami # prints current user
```

> You can also explore their options with `--help` (short) or `man <command_name>` (long).

---

## 3. Users, groups and permissions

Linux is a multi-user operating system (OS).

Each user has:

* a username (`whoami`)
* a primary group, usually named after the user
* possibly other groups (`groups`), such as `students`

---

## 3. Users, groups and permissions

Any file is owned by a user and a group, and has permissions specified by 9 bits: read (r), write (w) and execute (x) for the owning user, the owning group and others.

You can inspect them with `ls -l <file>` and `stat <file>`,
and set them with `chown <user>:<group> <file>` and `chmod <p> <file>`:

```bash
chown -R u1: dir   # dir and its contents are owned by user u1, group u1
chmod o-wx dir     # users who are not u1 and do not belong to group u1 cannot write or enter dir
chmod 700 dir      # u1 can read, write and execute (enter) dir
```

> 7 = 2^0 + 2^1 + 2^2 = r + w + x

> What happens with `rwx r-x rwx`?

---

## 4. File System Hierarchy

In `ls /` you find, among others:

* `/home`, containing users’ home directories; `~` is shorthand for `/home/$(whoami)`
* `/usr`, Unix System Resources, which contains:

  * `/usr/include` (check `echo $mkEigenInc` in pacs env)
  * `/usr/lib` (check `echo $mkTbbLib` in pacs env)
* `/bin` binaries, see also `ls / -l | grep bin`
* `/opt` optional software packages (local FS)

---

## 4. File System Hierarchy

You can inspect disk space usage with `df` and `du`:

```bash
df -h        # shows used and available space on mounted file systems (-h: human-readable units)
du -d 1 -h . # shows disk usage of current directory and its one-level-deep subdirectories
```

See also `lsblk` for local block devices only.

---

## 5. Network File System

NFS (Network File System) allows a computer to access files stored on another server over a network: the server shares a directory, and clients mount it into their own filesystem.

In a cluster using NFS over Ethernet, NFS-exported file systems are shared across nodes, so your files are visible over the network.

However, performance depends on the network and the NFS server, so heavy I/O (many reads/writes) can become slow or create contention.

---

## 6. Scratch

That’s why `/local_scratch` is local to each node — it’s much faster and better for temporary, high-I/O job data, but it is not shared and may be periodically cleaned.

Check the relevant policies in:

```bash
ls /etc/tmpfiles.d/
```

Currently, files in scratch older than 30 days are removed.

---

## 7. Open Portable Batch System

OpenPBS is software that optimises job scheduling and workload management in high-performance computing environments.

### First steps:

* Set up SSH on the cluster
* Set up the PBS environment:

```bash
. /etc/profile.d/pbs.sh
```

The second step is optional. You can use the full path instead, e.g.:

```bash
/opt/pbs/bin/pbsnodes -aSj
```

---

## 7. Open Portable Batch System

### Main Commands

* `pbsnodes -aSj` → check available nodes
* `qstat` → inspect queues
* `qsub` → submit jobs
* `qdel` → delete submitted jobs

Base submission command:

```bash
qsub -I
```

Requests an interactive job with default options (`ncpus=1`).

Use `Ctrl+D` to exit.

---

## 7. Open Portable Batch System

You can submit batch jobs via submission scripts.

Example interactive request:

```bash
qsub -I -l select=1:ncpus=2:host=cpu03
```

Requests:

* 1 node
* 2 CPUs (threads)
* Host `cpu03`

Unspecified parameters (queue, walltime) use default values.

---

## 8. Software

Each node has both locally installed software and software shared via NFS (under `/software`).

### Local Software

Besides essentials (e.g. `gcc`, `git`, `vim`, `nano`, `make`, `python`), some software is installed locally, such as:

* **Apptainer**
* **GNU Octave**
* **FreeFem++** (installed only on CPU nodes)

These can be launched directly.

---

## 8. Software

### Spack

Various Spack installations are available under `/software`.

A link to the default one is:

```bash
/software/spack
```

See the Spack repository:

[https://github.com/spack/spack](https://github.com/spack/spack)

---

## 8. Software

### Example: Load a Modern C++ Toolchain with TBB and OpenMPI

```bash
source /software/spack/share/spack/setup-env.sh
spack load gcc@15.2.0
spack load intel-oneapi-tbb@2022.3.0
spack load openmpi@5.0.8
```

You can verify successful loading with:

```bash
gcc --version
which gcc
```

---

## 8. Software

### Useful Spack Commands

* List available packages:

```bash
spack find
```

* Unload all loaded packages:

```bash
spack unload --all
```

---

## 8. Software

### Apptainer

`/software/containers` contains various `apptainer` images.

More can be built from Docker or Podman images — for example from [Docker Hub](https://hub.docker.com/), [Quay.io](https://quay.io/), and [NVIDIA NGC](https://catalog.ngc.nvidia.com/) — or from text files called recipes.

We'll cover these topic in the last labs; in the meantime you can refer to:

[https://github.com/pacs-course/pacs-Labs/tree/main/Labs/2025/15-spack%2Bcontainers](https://github.com/pacs-course/pacs-Labs/tree/main/Labs/2025/15-spack%2Bcontainers)

---

## 8. Software

### Python environments

You can create your own virtual environments or use an existing one.

For example, the environment `/software/pyplot` can be used to produce Matplotlib plots via the Jupyter CLI:

```bash
source /software/pyplot/bin/activate
nbterm notebook.ipynb
deactivate
```

See the `nbterm` documentation:

[https://github.com/davidbrochart/nbterm](https://github.com/davidbrochart/nbterm)

---
## 8. Software
### Creating a New Python Virtual Environment

Example:

```bash
python -m venv ~/my_env
source ~/my_env/bin/activate
pip install --upgrade pip
pip install numpy matplotlib jupyter nbterm
```

If a specific Python version is required, ensure that version is available on the system before creating the virtual environment, or install it with Spack (you can also clone your own Spack repository — see `git remote -v` in `/software/spack`).

---

## 9. Exercise: pacs-labs (HPC cluster workflow)

We’ll run:

[https://github.com/pacs-course/pacs-Labs/tree/main/Labs/2025/16-MPI-multi-thread%2BJupyter](https://github.com/pacs-course/pacs-Labs/tree/main/Labs/2025/16-MPI-multi-thread%2BJupyter)

as **cluster workflow example**, without going through the specific programming application — we’ll work directly with the solution.

---
## 9. Exercise: pacs-labs (HPC cluster workflow)
### 1. Ask for an interactive job

```bash
. /etc/profile.d/pbs.sh
pbsnodes -aSj
qsub -I -q cpu -l select=1:ncpus=6:mpiprocs=2:host=cpu04
```

---
## 9. Exercise: pacs-labs (HPC cluster workflow)
### 2. Move into `/scratch_local` and clone pacs-labs

```bash
cd /scratch_local
df -h .
mkdir $(whoami)
cd $(whoami)
git clone -c feature.manyFiles=true --depth=1 \
https://github.com/pacs-course/pacs-labs.git
```

---
## 9. Exercise: pacs-labs (HPC cluster workflow)
### 3. Load the required Spack packages

```bash
source /software/spack/share/spack/setup-env.sh
spack load gcc@15.2.0
spack load intel-oneapi-tbb@2022.3.0
spack load openmpi@5.0.8
```

---
## 9. Exercise: pacs-labs (HPC cluster workflow)
### 4. Activate the required Python environment
   (see previous slides for creation)

```bash
source /software/pyplot/bin/activate
```

---
## 9. Exercise: pacs-labs (HPC cluster workflow)
### 5. Open the notebook

```bash
cd pacs-labs/Labs/2025/16-MPI-multi-thread+Jupyter
nbterm 16-MPI-multi-thread+jupyter.ipynb
```

and run the gcc-based (`mpicxx`) example.

> Ctrl+H for help

> The first output line should be `g++ (Spack GCC) 15.2.0`,
> the last `Plotting grid 512x256, t = 0.048828125`

---
## 9. Exercise: pacs-labs (HPC cluster workflow)
### 6. Exit the notebook, then check and save the output to a global NFS file system

```bash
Ctrl+Q
Ctrl+Q
ls -larth
cp output.png ~/
cd
ls -larth
```

---
## 9. Exercise: pacs-labs (HPC cluster workflow)
### 7. Terminate the job, log out, copy the output locally, and inspect it on your PC

```bash
Ctrl+D
Ctrl+D
scp u<person_code>@<IP>:/home/u<person_code>/output.png .
xdg-open ./output.png &  # or use your GUI
```

---

## 10. Q&A

