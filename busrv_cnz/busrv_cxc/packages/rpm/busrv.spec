#
# spec file for configuration of package BUSRV service
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#                                     giovanni.gambardella@ericsson.com
#

Name:      %{_name}
Summary:   Installation package for BUSRV Service.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Application
BuildRoot: %_tmppath

Requires: APOS_OSCONFBIN
# This will make rpm not find the dependencies 
# TODO: should ignore dependecies?
Autoreq: 0

%define busrv_cxc_path %{_cxcdir}
%define busrv_cxc_bin %{busrv_cxc_path}/bin
%define busrv_lib_path %{busrv_cxc_bin}/lib_ext
%define busrv_cxc_conf %{busrv_cxc_path}/conf
%define busrv_conf_path %{CPSCONFdir}/busrv
%define cps_data_path /data/cps/data

%description
Installation package for BUSRV Service.

# ---------------
# preinstallation
# ---------------
%pre
##echo "This is the BUSRV Service package pre-install section"
if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} preinstall script during installation phase"

echo "... Commands to be executed only during installation phase in pre section..."

fi

if [ $1 == 2 ]
then
echo "This is the %{_name} package %{_rel} preinstall script during upgrade phase"
echo "... Commands to be executed only during Upgrade phase in pre section..."

rm -f %{_bindir}/cps_busrvd
rm -f %{_bindir}/bupdef
rm -f %{_bindir}/bupidls
rm -f %{_bindir}/bupls
rm -f %{_bindir}/cps_busrv_busrvservice_clc
rm -f %{_bindir}/bupidls.sh
rm -f %{_lib64dir}/libcps_sbc.so
rm -f %{_lib64dir}/libcps_sbc.so.1

fi

echo "... Commands to be executed only during both phases in pre section..."

# ---------------
# installation
# ---------------
%install
##echo "This is the BUSRV Service package install section"
echo "This is the %{_name} package %{_rel} install script"

# Create necessary directories
#rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{CPSBINdir}
mkdir $RPM_BUILD_ROOT/%{CPSLIBdir}
mkdir $RPM_BUILD_ROOT/%{CPSETCdir}
mkdir $RPM_BUILD_ROOT/%{CPSCONFdir}
mkdir -p $RPM_BUILD_ROOT/%{busrv_conf_path}
mkdir $RPM_BUILD_ROOT/%{CPSLIB64dir}

# Copy binaries & conf files to appropriate directories
cp %{busrv_cxc_bin}/cps_busrvd  $RPM_BUILD_ROOT/%{CPSBINdir}/cps_busrvd
cp %{busrv_cxc_bin}/bupdef      $RPM_BUILD_ROOT/%{CPSBINdir}/bupdef
cp %{busrv_cxc_bin}/bupidls     $RPM_BUILD_ROOT/%{CPSBINdir}/bupidls
cp %{busrv_cxc_bin}/bupls       $RPM_BUILD_ROOT/%{CPSBINdir}/bupls
cp %{busrv_cxc_bin}/cps_busrv_busrvservice_clc       $RPM_BUILD_ROOT/%{CPSBINdir}/cps_busrv_busrvservice_clc
cp %{busrv_cxc_bin}/bupidls.sh     $RPM_BUILD_ROOT/%{CPSBINdir}/bupidls.sh

cp %{busrv_cxc_path}/conf/busrv.dat      $RPM_BUILD_ROOT/%{busrv_conf_path}/busrv.dat
cp %{busrv_lib_path}/libcps_sbc.so.1.1.0 $RPM_BUILD_ROOT/%{CPSLIB64dir}/libcps_sbc.so.1.1.0

%post
##echo "This is the BUSRV service package post-install section"
echo "This is the %{_name} package %{_rel} post-install script"

chmod +x %{CPSBINdir}/cps_busrvd
chmod +x %{CPSBINdir}/bupdef
chmod +x %{CPSBINdir}/bupidls
chmod +x %{CPSBINdir}/bupls
chmod +x %{CPSBINdir}/cps_busrv_busrvservice_clc
chmod +x %{CPSBINdir}/bupidls.sh
chmod +x %{CPSLIB64dir}/libcps_sbc.so.1.1.0

ln -sf %{CPSBINdir}/cps_busrvd %{_bindir}/cps_busrvd
ln -sf %{CPSBINdir}/bupdef %{_bindir}/bupdef
ln -sf %{CPSBINdir}/bupidls.sh %{_bindir}/bupidls
ln -sf %{CPSBINdir}/bupls %{_bindir}/bupls
ln -sf %{CPSLIB64dir}/libcps_sbc.so.1.1.0  %{_lib64dir}/libcps_sbc.so.1
ln -sf %{_lib64dir}/libcps_sbc.so.1  %{_lib64dir}/libcps_sbc.so

if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} postinstall script during installation phase"
echo "... Commands to be executed only during installation phases in post section..."
	# Only copy config file when /data/cps/data exists during installation only
	if [ -d %{cps_data_path} ]
	then
	    mkdir -p %{cps_data_path}/busrv/conf
	    cp %{busrv_conf_path}/busrv.dat %{cps_data_path}/busrv/busrv.dat
	    # Change to write permisson for these files
	    chmod +w %{cps_data_path}/busrv/busrv.dat
	fi
fi

# Remove config file
rm -f %{busrv_conf_path}/busrv.dat

%preun
##echo "This is the BUSRV service package pre-uninstall section"
## do nothing 
if [ $1 == 0 ]
then
echo "This is the %{_name} package %{_rel} preun script during unistall phase"
echo "... Commands to be executed only during uninstallation phases in preun section..."

rm -f %{_bindir}/cps_busrvd
rm -f %{_bindir}/bupdef
rm -f %{_bindir}/bupidls
rm -f %{_bindir}/bupls
rm -f %{_lib64dir}/libcps_sbc.so
rm -f %{_lib64dir}/libcps_sbc.so.1

fi

if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} preun script during upgrade phase"
echo "... Commands to be executed only during installation phases in preun section..."

fi

%postun
##echo "This is the BUSRV service package post-uninstall section"
if [ $1 == 0 ]
then
echo "This is the %{_name} package %{_rel} postun script during unistall phase"
echo "... Commands to be executed only during uninstallation phases in postun section..."
fi

if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} postun script during upgrade phase"
echo "... Commands to be executed only during upgrade phases in postun section..."
fi

%files
%defattr(-,root,root)
%{CPSBINdir}/cps_busrvd
%{CPSBINdir}/bupdef
%{CPSBINdir}/bupidls
%{CPSBINdir}/bupls
%{CPSBINdir}/cps_busrv_busrvservice_clc
%{CPSBINdir}/bupidls.sh
%{CPSLIB64dir}/libcps_sbc.so.1.1.0
%{busrv_conf_path}/busrv.dat

%changelog
* Mon Mar 4 2013 - danh.nguyen (at) dektech.com.au
- Added busrv library 
* Mon Oct 1 2012 - danh.nguyen (at) dektech.com.au
- Update with S-impact
* Fri Jun 1 2012 - quyen.dao (at) dektech.com.au
- Remove busrv.ini & PHA file
* Tue Apr 10 2012 - quyen.dao (at) dektech.com.au
- Remove HA xml file
* Thu Sep 19 2011 - quyen.dao (at) dektech.com.au
- Change the clc script & objects.xml name
* Thu Jul 7 2011 - thanh.nguyen (at) dektech.com.au
- Upgdate to distinguish between maiden installation and upgrade
* Wed Jun 1 2011 - quyen.dao (at) dektech.com.au
- Added CLC & XML file for AMF. Note: those files may need to take out to SDP package in the future
* Wed Apr 13 2011 - quyen.dao (at) dektech.com.au
- Changed write permisson to these files: busrv.ini & busrv.dat
* Wed Apr 13 2011 - quyen.dao (at) dektech.com.au
- Removed creating directory section (it will be handled by STP installation script)
* Wed Mar 17 2011 - quyen.dao (at) dektech.com.au
- Update for SDK_APG43L_PROJ_0504_006
* Wed Mar 2 2011 - quyen.dao (at) dektech.com.au
- Update for bu cmds packaging
* Wed Feb 23 2011 - quyen.dao (at) dektech.com.au
- Update the busrv daemon name to comply with DR
* Fri Feb 11 2011 - quyen.dao (at) dektech.com.au
- Update for SDK_APG43L_PROJ_0504_002

