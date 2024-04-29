#!/bin/bash
# Update Qt version on CentOS6

KDE_FILE_CONTENT='# kde.repo, v2.X
# Access a specific reliable mirror for Qt package updates -- Nüvü Cameras, Inc.

[kde]
name=kde
mirrorlist=https://ftp.heanet.ie/mirrors/kde-redhat/redhat/mirrors-stable
gpgkey=https://ftp.heanet.ie/mirrors/kde-redhat/kde-redhat.RPM-GPG-KEY
#gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-kde-redhat
enabled=1
skip_if_unavailable=1

# NOTE: requires/depends on kde repo(s)
[kde-testing]
name=kde-testing
mirrorlist=http://apt.kde-redhat.org/apt/kde-redhat/redhat/mirrors-testing
gpgkey=http://apt.kde-redhat.org/apt/kde-redhat/kde-redhat.RPM-GPG-KEY
#gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-kde-redhat
enabled=0
skip_if_unavailable=1

# NOTE: requires/depends-on testing repo(s)
[kde-unstable]
name=kde-unstable
mirrorlist=http://apt.kde-redhat.org/apt/kde-redhat/redhat/mirrors-unstable
gpgkey=http://apt.kde-redhat.org/apt/kde-redhat/kde-redhat.RPM-GPG-KEY
#gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-kde-redhat
enabled=0
skip_if_unavailable=1'

if [ -f /etc/yum.repos.d/kde.repo ]
then # Already tried?
	# May be out-of-date: start fresh
	yum-config-manager --disable kde
	rm -i /etc/yum.repos.d/kde.repo
fi

echo "$KDE_FILE_CONTENT" > ./kde.repo
yum-config-manager --add-repo ./kde.repo
rm  -f ./kde.repo
#yum upgrade kdebase # Helps dependencies get over a name-change of this package to kde-baseapps
#                      ... but likely brings in unnecessary (and potentially conflicting) dependency on samba

yum update qt 
yum-config-manager --disable kde

