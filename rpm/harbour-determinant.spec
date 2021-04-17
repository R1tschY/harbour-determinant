Name:       harbour-determinant
Summary:    A Sailfish OS Matrix Client
Version:    0.3.0
Release:    1
Group:      Qt/Qt
License:    LICENSE
URL:        https://github.com/R1tschY/harbour-determinant
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(sailfishsecrets)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(nemonotifications-qt5)
BuildRequires:  desktop-file-utils

%description
Matrix client for Sailfish OS.


%prep
%setup -q -n %{name}-%{version}

%build
%qmake5 

make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
