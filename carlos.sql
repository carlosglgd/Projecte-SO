DROP DATABASE IF EXISTS carlos;

CREATE DATABASE carlos;

USE carlos;



CREATE TABLE jugador(

username varchar(15) primary key NOT NULL,

pass varchar(10) NOT NULL,
mail varchar(30) NOT NULL

)ENGINE = InnoDB;



CREATE TABLE partida(

id integer primary key not null,

fechahora varchar(25) not null,

duracion varchar(25) not null,

ganador varchar(15) not null,

FOREIGN KEY (ganador) REFERENCES jugador(username)

)engine = innodb;



CREATE TABLE RelacionJugadorPartida(
player varchar(15),
id_partida integer,
FOREIGN KEY (ganador) REFERENCES jugador(username),
FOREIGN KEY (id) REFERENCES partida(id_partida)
)ENGINE = innodb;





 insert into jugador(username, pass, mail) values('Carlos', 1, 'mail1');

 insert into jugador(username, pass, mail) values('Jose', 2, 'mail2');

 insert into jugador(username, pass, mail) values('Ines', 3, 'mail3');

 insert into jugador(username, pass, mail) values('Blanca', 4, 'mail4');

 insert into jugador(username, pass, mail) values('Pilar', 5, 'mail5');

 insert into jugador(username, pass, mail) values('Albert', 6, 'mail6');



insert into partida(id, fechahora, duracion, ganador) values(1,'2010-01-01 00:01 00:00:01', '05:40:32', 'Carlos');

insert into partida(id, fechahora, duracion, ganador) values(2,'2011-01-01 00:01 00:00:01', '05:40:32', 'Carlos');

insert into partida(id, fechahora, duracion, ganador) values(3,'2012-01-01 00:01 00:00:01', '05:40:32', 'Carlos');

insert into partida(id, fechahora, duracion, ganador) values(4,'2013-01-01 00:01 00:00:01', '05:40:32', 'Ines');





insert into RelacionJugadorPartida(player, id_partida) values ('Carlos', 1);

insert into RelacionJugadorPartida(player, id_partida) values ('Blanca', 1);

insert into RelacionJugadorPartida(player, id_partida) values ('Carlos', 2);

insert into RelacionJugadorPartida(player, id_partida) values ('Blanca', 3);

insert into RelacionJugadorPartida(player, id_partida) values ('Carlos', 3);

insert into RelacionJugadorPartida(player, id_partida) values ('Blanca', 4);

insert into RelacionJugadorPartida(player, id_partida) values ('Carlos', 5);



