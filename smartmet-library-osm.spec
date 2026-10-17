%define DIRNAME osm
%define LIBNAME smartmet-%{DIRNAME}
%define SPECNAME smartmet-library-%{DIRNAME}
Summary: OpenStreetMap place data library
Name: %{SPECNAME}
Version: 26.10.17
Release: 1%{?dist}.fmi
License: MIT
Group: Development/Libraries
URL: https://github.com/fmidev/smartmet-library-osm
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot-%(%{__id_u} -n)

%if 0%{?rhel} && 0%{rhel} < 9
%define smartmet_boost boost169
%else
%define smartmet_boost boost
%endif

BuildRequires: %{smartmet_boost}-devel
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: rpm-build
BuildRequires: smartmet-library-macgyver-devel >= 26.10.16
Requires: %{smartmet_boost}-system
Requires: %{smartmet_boost}-thread
Requires: smartmet-library-macgyver >= 26.10.17

#TestRequires: %{smartmet_boost}-devel
#TestRequires: gcc-c++
#TestRequires: make
#TestRequires: smartmet-library-macgyver-devel >= 26.10.16
#TestRequires: smartmet-library-regression
Provides: %{SPECNAME}

%description
Library for reading OpenStreetMap-derived place data (cities, towns,
villages, hamlets) from a local mirror, intended for use by
smartmet-engine-geonames as an alternative backend to GeoNames.

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n %{SPECNAME}

%build
make %{_smp_mflags}

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(0775,root,root,0775)
%{_libdir}/libsmartmet-%{DIRNAME}.so

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%package -n %{SPECNAME}-devel
Summary: smartmet-library-osm development files
Provides: %{SPECNAME}-devel
Requires: %{SPECNAME}

%description -n %{SPECNAME}-devel
smartmet-library-osm development files
Requires: %{SPECNAME} = %version-%release
Requires: smartmet-library-macgyver-devel

%files -n %{SPECNAME}-devel
%defattr(0664,root,root,0775)
%{_includedir}/smartmet/%{DIRNAME}

%changelog
* Sat Oct 17 2026 Mika Heiskanen <mika.heiskanen@fmi.fi> - 26.10.17-1.fmi
- Split functions with a cognitive complexity above 25 into smaller helpers, no functional changes

* Fri Oct 16 2026 Mika Heiskanen <mika.heiskanen@fmi.fi> - 26.10.16-1.fmi
- Fixed compiler warnings and clang-tidy findings (unused code, unreachable code, empty statements, member initialization, parameter passing, noexcept), no functional changes
- ABI change: parameters passed by const reference or by value, removed unused members and defaulted copy operations, dependent packages must be rebuilt

* Tue Apr 28 2026 Mika Heiskanen <mika.heiskanen@fmi.fi> - 26.4.28-1.fmi
- Initial version with file-based DataSource and a stub Postgres backend
