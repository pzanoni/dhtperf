Name:    dhtperf
Summary: Distributed Hash Table PERFormance measurement tool
Version: 0.10.0
Release: 1%{?dist}
License: GPLv2
URL:     http://gitorious.org/dhtperf
Group:   Development/Tools

Source0: dhtperf-master
Source1: dhtperf-controller
Source2: dhtperfd
Source3: dhtperfd-client
Source4: dhtperfd-service
Source5: workloads

Source10: openchord-console
Source11: openchord_1.0.5.jar
Source12: log4j.jar

Source20: owdhtshell
Source21: overlayweaver.tar.bz2

Source30: past-console
Source31: PastConsole.jar
Source32: pastry.jar
Source33: xmlpull_1_1_3_4a.jar
Source34: xpp3-1.1.3.4d_b2.jar

# These should all be "automatically" required, but let's add them too:
#Requires: boost-date-time
#Requires: boost-program-options
#Requires: boost-regex
#Requires: boost-system
#Requires: boost-thread
Requires: java

#BuildRequires: cmake
#BuildRequires: boost-devel
#BuildRequires: ant
#BuildRequires: java-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}

%description
Package used for PlanetLab testing. Should not be used anywhere else.

%prep

%build

%install
rm -rf $RPM_BUILD_ROOT
install -d $RPM_BUILD_ROOT/%{_bindir}
install -d $RPM_BUILD_ROOT/%{_datadir}/dhtperf/openchord
install -d $RPM_BUILD_ROOT/%{_datadir}/dhtperf/overlayweaver
install -d $RPM_BUILD_ROOT/%{_datadir}/dhtperf/pastconsole
install -d $RPM_BUILD_ROOT/%{_initrddir}

# dhtperf
install %{_sourcedir}/dhtperf-master     $RPM_BUILD_ROOT/%{_bindir}
install %{_sourcedir}/dhtperf-controller $RPM_BUILD_ROOT/%{_bindir}
install %{_sourcedir}/dhtperfd           $RPM_BUILD_ROOT/%{_bindir}
install %{_sourcedir}/dhtperfd-client    $RPM_BUILD_ROOT/%{_bindir}
install %{_sourcedir}/dhtperfd-service   $RPM_BUILD_ROOT/%{_initrddir}/dhtperfd
cp -r   %{_sourcedir}/workloads          $RPM_BUILD_ROOT/%{_datadir}/dhtperf/

# openchord
install %{_sourcedir}/openchord-console   $RPM_BUILD_ROOT/%{_bindir}
install %{_sourcedir}/openchord_1.0.5.jar \
	$RPM_BUILD_ROOT/%{_datadir}/dhtperf/openchord/
install %{_sourcedir}/log4j.jar $RPM_BUILD_ROOT/%{_datadir}/dhtperf/openchord/

# overlayweaver
install %{_sourcedir}/owdhtshell $RPM_BUILD_ROOT/%{_bindir}
tar xjf %{_sourcedir}/overlayweaver.tar.bz2 -C \
	$RPM_BUILD_ROOT/%{_datadir}/dhtperf/

# pastconsole
install %{_sourcedir}/past-console $RPM_BUILD_ROOT/%{_bindir}
install %{_sourcedir}/PastConsole.jar \
	$RPM_BUILD_ROOT/%{_datadir}/dhtperf/pastconsole/
install %{_sourcedir}/pastry.jar \
	$RPM_BUILD_ROOT/%{_datadir}/dhtperf/pastconsole/
install %{_sourcedir}/xmlpull_1_1_3_4a.jar \
	$RPM_BUILD_ROOT/%{_datadir}/dhtperf/pastconsole/
install %{_sourcedir}/xpp3-1.1.3.4d_b2.jar \
	$RPM_BUILD_ROOT/%{_datadir}/dhtperf/pastconsole/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/dhtperf-master
%{_bindir}/dhtperf-controller
%{_bindir}/dhtperfd
%{_bindir}/dhtperfd-client
%{_bindir}/openchord-console
%{_bindir}/owdhtshell
%{_bindir}/past-console
%{_initrddir}/dhtperfd
%{_datadir}/dhtperf
