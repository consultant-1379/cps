#
# spec file for configuration of package cdh service prototype
#
# Copyright  (c)  2013  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#                                     giovanni.gambardella@ericsson.com
#

Name:      %{_name}
Summary:   Installation package for FCSRV.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     CPS
BuildRoot: %_tmppath

Requires: APOS_OSCONFBIN
AutoReqProv: no

%define fcsrv_cxc_path %{_cxcdir}
%define fcsrv_cxc_bin %{fcsrv_cxc_path}/bin
%define fcsrv_cxc_conf %{fcsrv_cxc_path}/conf

%description
Installation package for FCSRV.

%pre
if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} pre-install script during installation phase"
fi
if [ $1 == 2 ]
then
echo "This is the %{_name} package %{_rel} pre-install script during upgrade phase"
fi

%install
echo "This is the %{_name} package %{_rel} install script"

if [ ! -d $RPM_BUILD_ROOT%{rootdir} ]
then
    mkdir $RPM_BUILD_ROOT%{rootdir}
fi
if [ ! -d $RPM_BUILD_ROOT%{APdir} ]
then
    mkdir $RPM_BUILD_ROOT%{APdir}
fi
if [ ! -d $RPM_BUILD_ROOT%{CPSdir} ]
then
    mkdir $RPM_BUILD_ROOT%{CPSdir}
fi
if [ ! -d $RPM_BUILD_ROOT%{CPSBINdir} ]
then
    mkdir $RPM_BUILD_ROOT%{CPSBINdir}
fi
if [ ! -d $RPM_BUILD_ROOT%{CPSLIBdir} ]
then
    mkdir $RPM_BUILD_ROOT%{CPSLIBdir}
fi
if [ ! -d $RPM_BUILD_ROOT%{CPSLIB64dir} ]
then
    mkdir $RPM_BUILD_ROOT%{CPSLIB64dir}
fi
if [ ! -d $RPM_BUILD_ROOT%{CPSETCdir} ]
then
    mkdir $RPM_BUILD_ROOT%{CPSETCdir}
fi
if [ ! -d $RPM_BUILD_ROOT%{CPSCONFdir} ]
then
    mkdir $RPM_BUILD_ROOT%{CPSCONFdir}
fi

cp %{fcsrv_cxc_bin}/cps_fcsrvd $RPM_BUILD_ROOT%{CPSBINdir}/cps_fcsrvd
cp %{fcsrv_cxc_bin}/cps_fcsrv_fcsrvservice_clc $RPM_BUILD_ROOT%{CPSBINdir}/cps_fcsrv_fcsrvservice_clc

%post
if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} post script during installation phase"

ln -sf %{CPSBINdir}/cps_fcsrvd  %{_bindir}/cps_fcsrvd

fi

if [ $1 == 2 ]
then
echo "This is the %{_name} package %{_rel} post script during upgrade phase"
fi

# For both post phases
chmod +x  %{CPSBINdir}/cps_fcsrvd
chmod +x  %{CPSBINdir}/cps_fcsrv_fcsrvservice_clc

%preun
if [ $1 == 0 ]
then
echo "This is the %{_name} package %{_rel} preun script during unistall phase"

fi

if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} preun script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
echo "This is the %{_name} package %{_rel} postun script during unistall phase"
rm -f %{_bindir}/cps_fcsrvd
rm -f %{_bindir}/cps_fcsrv_fcsrvservice_clc

fi

if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} postun script during upgrade phase"
fi

%files
%defattr(-,root,root)
%{CPSBINdir}/cps_fcsrvd
%{CPSBINdir}/cps_fcsrv_fcsrvservice_clc

%changelog
* Thu Jan 10 2013 - danh.nguyen (at) dektech.com.au
- Created for fcsrv
* Mon Jul 01 2010 - giovanni.gambardella (at) ericsson.com
- Initial implementation

