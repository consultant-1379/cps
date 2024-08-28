#
# spec file for configuration of package cdh service prototype
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#                                     giovanni.gambardella@ericsson.com
#

Name:      %{_name}
Summary:   Installation package for BUAP.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     CPS
BuildRoot: %_tmppath

Requires: APOS_OSCONFBIN
AutoReqProv: no

%define buap_cxc_path %{_cxcdir}
%define buap_cxc_bin %{buap_cxc_path}/bin
%define buap_cxc_conf %{buap_cxc_path}/conf

%description
Installation package for BUAP.

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

cp %{buap_cxc_bin}/cps_buapd $RPM_BUILD_ROOT%{CPSBINdir}/cps_buapd
cp %{buap_cxc_bin}/bupprint $RPM_BUILD_ROOT%{CPSBINdir}/bupprint
cp %{buap_cxc_bin}/bupset $RPM_BUILD_ROOT%{CPSBINdir}/bupset
cp %{buap_cxc_bin}/libcps_buap.so.2.1.1 $RPM_BUILD_ROOT%{CPSLIB64dir}/libcps_buap.so.2.1.1
cp %{buap_cxc_bin}/cps_buap_buapservice_clc $RPM_BUILD_ROOT%{CPSBINdir}/cps_buap_buapservice_clc
#cp %{buap_cxc_conf}/ha_cps_buap_buapservice_objects.xml $RPM_BUILD_ROOT%{CPSCONFdir}/ha_cps_buap_buapservice_objects.xml

%post
if [ $1 == 1 ] 
then
echo "This is the %{_name} package %{_rel} post script during installation phase"
fi

if [ $1 == 2 ]
then
echo "This is the %{_name} package %{_rel} post script during upgrade phase"
fi

# For both post phases
chmod +x  %{CPSBINdir}/cps_buapd
chmod +x  %{CPSBINdir}/bupprint
chmod +x  %{CPSBINdir}/bupset
chmod +x  %{CPSLIB64dir}/libcps_buap.so.2.1.1
chmod +x  %{CPSBINdir}/cps_buap_buapservice_clc
#chmod +x  %{CPSCONFdir}/ha_cps_buap_buapservice_objects.xml

ln -sf %{CPSBINdir}/cps_buapd  %{_bindir}/cps_buapd
ln -sf %{CPSBINdir}/bupprint  %{_bindir}/bupprint
ln -sf %{CPSBINdir}/bupset  %{_bindir}/bupset
ln -sf %{CPSLIB64dir}/libcps_buap.so.2.1.1  %{_lib64dir}/libcps_buap.so.2
ln -sf %{_lib64dir}/libcps_buap.so.2  %{_lib64dir}/libcps_buap.so
ln -sf %{CPSBINdir}/cps_buap_buapservice_clc  %{_bindir}/cps_buap_buapservice_clc

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
rm -f %{_bindir}/cps_buapd
rm -f %{_bindir}/bupprint
rm -f %{_bindir}/bupset
rm -f %{_lib64dir}/libcps_buap.so
rm -f %{_lib64dir}/libcps_buap.so.2
rm -f %{_bindir}/cps_buap_buapservice_clc
#rm -f %{_confdir}/ha_cps_buap_buapservice_objects.xml

fi

if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} postun script during upgrade phase"
fi

%files
%defattr(-,root,root)
%{CPSBINdir}/cps_buapd
%{CPSBINdir}/bupprint
%{CPSBINdir}/bupset
%{CPSLIB64dir}/libcps_buap.so.2.1.1
%{CPSBINdir}/cps_buap_buapservice_clc
#%{CPSCONFdir}/ha_cps_buap_buapservice_objects.xml

%changelog
* Fri Nov 04 2011 - danh.nguyen (at) dektech.com.au
- Updated with HA
* Mon Oct 17 2011 - tuan.nguyen (at) dektech.com.au
- Created for buap
* Mon Jul 01 2010 - giovanni.gambardella (at) ericsson.com
- Initial implementation

