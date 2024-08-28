#
# spec file for configuration of package BUFTPD commands
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#                                     giovanni.gambardella@ericsson.com
#

Name:      %{_name}
Summary:   Installation package for BUFTPD.
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

%define buftpd_cxc_path %{_cxcdir}
%define buftpd_cxc_bin %{buftpd_cxc_path}/bin

%description
Installation package for BUFTPD.

%pre
if [ $1 == 1 ] 
then
    echo "This is the %{_name} package %{_rel} pre-install script during installation phase"
fi

if [ $1 == 2 ]
then
    echo "This is the %{_name} package %{_rel} pre-install script during upgrade phase"
    # Remove the soft link created before
    rm -f %{_bindir}/cps_buftpd    
    # other real files will be overrided
fi

%install
echo "This is the %{_name} package %{_rel} install script"

# Create necessary directories if needed
#rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{CPSBINdir}
mkdir $RPM_BUILD_ROOT/%{CPSLIBdir}
mkdir $RPM_BUILD_ROOT/%{CPSETCdir}

# Copy buftpd command to appropiate directories
cp %{buftpd_cxc_bin}/cps_buftpd $RPM_BUILD_ROOT/%{CPSBINdir}/cps_buftpd
cp %{buftpd_cxc_bin}/cps_buftpd_buftpdservice_clc $RPM_BUILD_ROOT/%{CPSBINdir}/cps_buftpd_buftpdservice_clc

%post
if [ $1 == 1 ]
then
    echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi

if [ $1 == 2 ]
then
    echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x %{CPSBINdir}/cps_buftpd
chmod +x %{CPSBINdir}/cps_buftpd_buftpdservice_clc
ln -sf %{CPSBINdir}/cps_buftpd %{_bindir}/cps_buftpd

%preun
if [ $1 == 0 ]
then
    echo "This is the %{_name} package %{_rel} pre-uninstall script during unistall phase"
fi

if [ $1 == 1 ]
then
    echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
    echo "This is the %{_name} package %{_rel} post-uninstall script uninstall phase"
    # remove the soft link
    rm -f %{_bindir}/cps_buftpd    
fi

if [ $1 == 1 ]
then
    echo "This is the %{_name} package %{_rel} post-uninstall script upgrade phase"
fi

%files
%defattr(-,root,root)
%{CPSBINdir}/cps_buftpd
%{CPSBINdir}/cps_buftpd_buftpdservice_clc

%changelog
* Wed Oct 03 2012 - vu.m.nguyen (at) dektech.com.au
- APOSCONF dependency declaration
* Fri Apr 06 2012 - quyen.dao (at) dektech.com.au
- Removed AMF XML file
* Mon Oct 10 2011 - quyen.dao (at) dektech.com.au
- Updated for AMF
* Thu Jul 21 2011 - quyen.dao (at) dektech.com.au
- Created

