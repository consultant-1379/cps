#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#

Name:      %{_name}
Summary:   Installation package for CCH.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath
Requires: APOS_OSCONFBIN

#BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%define clog_cxc_path %{_cxcdir}

%description
Installation package for CLOG.

%pre

%install
echo "Installing CPS_CLOG package"

mkdir -p $RPM_BUILD_ROOT/opt/ap/cps/bin
mkdir -p $RPM_BUILD_ROOT/opt/ap/cps/conf

cp %clog_cxc_path/bin/cmdlls $RPM_BUILD_ROOT/opt/ap/cps/bin/cmdlls
cp %clog_cxc_path/bin/cmdlls.sh $RPM_BUILD_ROOT/opt/ap/cps/bin/cmdlls.sh
cp %clog_cxc_path/conf/cmdlls.ini $RPM_BUILD_ROOT/opt/ap/cps/conf/cmdlls.ini

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

echo "Finalizing CPS_CLOG package installation"
ln -sf /opt/ap/cps/bin/cmdlls $RPM_BUILD_ROOT/usr/bin/cmdlls
ln -sf /opt/ap/cps/bin/cmdlls.sh $RPM_BUILD_ROOT/usr/bin/cmdlls

#rm -Rf /cluster/FIXS/CCH/

mkdir -p $RPM_BUILD_ROOT/var/run/ap

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"		
		rm -f /usr/bin/cmdlls		
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"		
		rm -f /opt/ap/cps/bin/cmdlls
		rm -f /opt/ap/cps/bin/cmdlls.sh
        rm -f /opt/ap/cps/conf/cmdlls.ini
		#rm -Rf /cluster/etc/ap/fixs/cch/

fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%attr(555,root,root) /opt/ap/cps/bin/cmdlls
%attr(555,root,root) /opt/ap/cps/bin/cmdlls.sh
%attr(555,root,root) /opt/ap/cps/conf/cmdlls.ini
%changelog
* Tue Jun 18 2013 - xcsrpad
- Added command scripts
* Tue Jun 18 2013 - xcsrpad
- Initial implementation

