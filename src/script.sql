create database if not exists chat;
use chat;

create table user_info
(
    user_id        int auto_increment
        primary key,
    user_name      varchar(30) null,
    user_password  varchar(15) null,
    user_icon_path varchar(30) null
);

create table user_friend
(
    user_id   int         not null,
    friend_id int         not null,
    team      varchar(30) null,
    primary key (user_id, friend_id)
);

create table group_info
(
    group_id   int auto_increment
        primary key,
    group_name varchar(30) not null
);

create table group_member
(
    group_id  int not null,
    member_id int not null,
    primary key (group_id, member_id)
);

insert into user_info (user_name, user_password, user_icon_path)
values ('user1', 'qwerty', ''),
       ('user2', 'qwerty', ''),
       ('user3', 'qwerty', ''),
       ('user4', 'qwerty', ''),
       ('user5', 'qwerty', ''),
       ('user6', 'qwerty', ''),
       ('user7', 'qwerty', ''),
       ('user8', 'qwerty', ''),
       ('user9', 'qwerty', ''),
       ('user10', 'qwerty', ''),
       ('user11', 'qwerty', ''),
       ('user12', 'qwerty', '');

insert into group_info (group_name)
values ('group1'),
       ('group2'),
       ('group3'),
       ('group4'),
       ('group5');