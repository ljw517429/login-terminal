-- SET NAMES utf8;
-- SET FOREIGN_KEY_CHECKS=0;


DROP TABLE IF EXISTS `t_account_user_info`;
CREATE TABLE `t_account_user_info` (
  `dwId` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `strAccount`    varchar(256) NOT NULL,
  `strPassword`   varchar(256) NOT NULL,
  `strToken`      varchar(256) NOT NULL DEFAULT '',
  `ddCreatime`    bigint(20) unsigned NOT NULL,
  PRIMARY KEY(`dwId`),
  KEY(`strAccount`)
)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

-- SET FOREIGN_KEY_CHECKS=1;