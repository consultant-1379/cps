#
# spec file for configuration of package TESRV commands
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#                                     giovanni.gambardella@ericsson.com
#

Name:      %{_name}
Summary:   Installation package for TESRV commands.
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

%define tesrv_cxc_path %{_cxcdir}
%define tesrv_cxc_bin %{tesrv_cxc_path}/bin

%description
Installation package for TESRV commands.

%pre
if [ $1 == 1 ] 
then
    echo "This is the %{_name} package %{_rel} pre-install script during installation phase"
fi

if [ $1 == 2 ]
then
    echo "This is the %{_name} package %{_rel} pre-install script during upgrade phase"
    # Remove the soft link created before
    rm -f %{_bindir}/tesrvls
    rm -f %{_bindir}/tesrvtran
    # other real files will be overrided
fi

%install
echo "This is the %{_name} package %{_rel} install script"

# Create necessary directories if needed
#rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{CPSBINdir}
mkdir $RPM_BUILD_ROOT/%{CPSLIBdir}
mkdir $RPM_BUILD_ROOT/%{CPSETCdir}
mkdir $RPM_BUILD_ROOT/%{CPSCONFdir}

# Copy tesrvs command to appropiate directories
cp %{tesrv_cxc_bin}/tesrvls.sh $RPM_BUILD_ROOT/%{CPSBINdir}/tesrvls.sh
cp %{tesrv_cxc_bin}/tesrvls $RPM_BUILD_ROOT/%{CPSBINdir}/tesrvls
cp %{tesrv_cxc_bin}/tesrvtran.sh  $RPM_BUILD_ROOT/%{CPSBINdir}/tesrvtran.sh
cp %{tesrv_cxc_bin}/tesrvtran  $RPM_BUILD_ROOT/%{CPSBINdir}/tesrvtran

%post
if [ $1 == 1 ]
then
    echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi

if [ $1 == 2 ]
then
    echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x %{CPSBINdir}/tesrvls.sh
chmod +x %{CPSBINdir}/tesrvtran.sh
chmod +x %{CPSBINdir}/tesrvls
chmod +x %{CPSBINdir}/tesrvtran
ln -sf %{CPSBINdir}/tesrvls.sh %{_bindir}/tesrvls
ln -sf %{CPSBINdir}/tesrvtran.sh %{_bindir}/tesrvtran

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
    rm -f %{_bindir}/tesrvls
    rm -f %{_bindir}/tesrvtran
fi

if [ $1 == 1 ]
then
    echo "This is the %{_name} package %{_rel} post-uninstall script upgrade phase"
fi

%files
%defattr(-,root,root)
%{CPSBINdir}/tesrvls.sh
%{CPSBINdir}/tesrvtran.sh
%{CPSBINdir}/tesrvls
%{CPSBINdir}/tesrvtran

%changelog
* Wed Oct 03 2012 - vu.m.nguyen (at) dektech.com.au
- Updated to support upgrade
* Wed Jul 07 2011 - quyen.dao (at) dektech.com.au
- Updated to support upgrade
* Wed Apr 13 2011 - quyen.dao (at) dektech.com.au
- Created

