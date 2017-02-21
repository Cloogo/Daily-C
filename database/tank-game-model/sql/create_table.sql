use tankdb;
drop table if exists teams;
create table teams(
    name char(10),
    passwd char(10),
    captain char(10),
    soldier1 char(10),
    soldier2 char(10),
    soldier3 char(10),
    level int,
    vs_times int,
    win_times int,
    status char(10),
    rival char(10),
    invitation char(10)
);
drop table if exists users;
create table users(
    name char(10),
    team char(10),
    passwd char(10),
    title char(10),
    id int,
    status char(10),
    level int,
    blood int,
    offense int,
    defense int,
    exp int,
    x int,
    y int,
    bullet char(10),
    direction int
);

