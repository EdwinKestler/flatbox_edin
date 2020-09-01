CREATE DATABASE Edin_DB;

USE Edin_DB;

CREATE TABLE Modules(
    idModule INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
    module VARCHAR(128)
);

CREATE TABLE Users(
    gloveCode VARCHAR(128) PRIMARY KEY UNIQUE,
    calculusPoints INT NOT NULL,
    abstractPoints INT NOT NULL,
    memoryPoints INT NOT NULL,
    discernmentPoints INT NOT NULL,
    streaks INT,
    negativeStreaks INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

CREATE TABLE UsersModule(
    gloveCode VARCHAR(128) NOT NULL,
    idModule INT NOT NULL,
    PRIMARY KEY(gloveCode,idModule)
);

CREATE TABLE Operations(
    idOperation INT PRIMARY KEY NOT NULL AUTO_INCREMENT,
    idModule INT NOT NULL,
    gloveCode VARCHAR(128),
    number1 DOUBLE,
    operation VARCHAR(1),
    number2 DOUBLE,
    result DOUBLE,
    answer INT,
    ACTION VARCHAR(32),
    topic VARCHAR(128),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (idModule) REFERENCES Modules(idModule),
    FOREIGN KEY (gloveCode) REFERENCES Users(gloveCode)
);

CREATE TABLE Responses(
   idOperation INT NOT NULL,
   gloveCode VARCHAR(128),
   answer INT,
   state BOOL,
   created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ,
   updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   FOREIGN KEY(idOperation) REFERENCES Operations(idOperation)
);

DELIMITER //
CREATE PROCEDURE SP_CreateUser(
  _gloveCode VARCHAR(128)
)
BEGIN	
	INSERT INTO Users(gloveCode,calculusPoints,abstractPoints,memoryPoints,discernmentPoints,streaks,negativeStreaks) VALUES
	(_gloveCode,0,0,0,0,0,0);
	
	SELECT * FROM Users WHERE gloveCode = _gloveCode;   
 END//
DELIMITER ;

DELIMITER //
CREATE PROCEDURE SP_CreateQuestion(
  _idModule INT,
  _gloveCode VARCHAR(128),
  _number1 DOUBLE,
  _operation VARCHAR(1),
  _number2 DOUBLE,
  _result DOUBLE,
  _answer DOUBLE,
  _action VARCHAR(32),
  _topic VARCHAR(128)
)
BEGIN
	INSERT INTO Operations(idModule,gloveCode,number1,operation,number2,result,answer,ACTION,topic) VALUES (_idModule,_gloveCode,_number1,_operation,_number2,_result,_answer,_action,_topic);
	SET @idOperation = LAST_INSERT_ID();
	SELECT * FROM Operations WHERE idOperation = @idOperation;   
 END//
DELIMITER ;

DELIMITER //
CREATE PROCEDURE SP_GetPoints(
  _idModule INT,
  _gloveCode VARCHAR(128)
)
BEGIN
    SET @countQuestions = (SELECT COUNT(*) FROM Operations WHERE gloveCode = _gloveCode AND idModule = _idModule);
    
    SET @correctAnswers = (SELECT COUNT(*) FROM Responses WHERE state = TRUE AND gloveCode = _gloveCode);
    
    IF @countQuestions > 0 AND @correctAnswers > 0 THEN
        SET @currentPoints = (@correctAnswers / @countQuestions) * 100;
        SELECT @currentPoints;
    ELSE
        SET @currentPoints = 0;
        SELECT @currentPoints;
    END IF; 
 END//
DELIMITER ;

DELIMITER //
CREATE PROCEDURE SP_ResponseQuestion(
  _idOperation INT,
  _gloveCode VARCHAR(128),
  _answer INT
)
BEGIN
	SET @topic = (SELECT operation FROM Operations WHERE idOperation = _idOperation);
	SET @negativeStreaks = (SELECT negativeStreaks FROM Users WHERE gloveCode = _gloveCode);
	SET @streaks = (SELECT streaks FROM Users WHERE gloveCode = _gloveCode);
	SET @result = (SELECT result FROM Operations WHERE idOperation = _idOperation);
	SET @operation = (SELECT operation FROM Operations WHERE idOperation = _idOperation);
	SET @calculusPoints = (SELECT calculusPoints FROM Users WHERE gloveCode = _gloveCode);
	SET @abstractPoints = (SELECT abstractPoints FROM Users WHERE gloveCode = _gloveCode);
	SET @memoryPoints = (SELECT memoryPoints FROM Users WHERE gloveCode = _gloveCode);
	SET @discernmentPoints = (SELECT discernmentPoints FROM Users WHERE gloveCode = _gloveCode);
	
	IF @result = _answer THEN
	    SET @state = TRUE;	
	ELSE
	    SET @state = FALSE;
	END IF;
	INSERT INTO Responses(idOperation,gloveCode,answer,state) VALUES (_idOperation,_gloveCode,_answer,@state); 
		
	SET @countQuestions = (SELECT COUNT(*) FROM Operations WHERE topic = 'all' OR topic = _gloveCode);		
	
	IF @countQuestions > 0 THEN
	   IF @state = TRUE THEN
	   	IF @operation = '*' OR @operation = '/' AND @abstractPoints < 98 THEN
	           UPDATE Users SET abstractPoints = @abstractPoints:=(@abstractPoints + 2) WHERE gloveCode = _gloveCode;
	        ELSEIF @operation = '*' OR @operation = '/' THEN
	           UPDATE Users SET abstractPoints = 100 WHERE gloveCode = _gloveCode;
	        END IF;
	        
	        IF @operation = '+' OR @operation = '-' AND @memoryPoints < 95 THEN
	           UPDATE Users SET memoryPoints = @memoryPoints:=(@memoryPoints + 5) WHERE gloveCode = _gloveCode;
	        ELSEIF @operation = '+' OR @operation = '-' THEN
	           UPDATE Users SET memoryPoints = 100 WHERE gloveCode = _gloveCode;
	        END IF;
	        
		IF @calculusPoints < 97 THEN
		   UPDATE Users SET calculusPoints = @calculusPoints:=(@calculusPoints + 3) WHERE gloveCode = _gloveCode;	
		ELSE		
		   UPDATE Users SET calculusPoints = 100 WHERE gloveCode = _gloveCode;
		END IF;
		
	        UPDATE Users SET streaks = streaks + 1 WHERE gloveCode = _gloveCode;
	        UPDATE Users SET negativeStreaks = 0 WHERE gloveCode = _gloveCode;
	        
	        IF @discernmentPoints < 95 THEN 
		   IF @streaks = 3 THEN
		      UPDATE Users SET discernmentPoints = @discernmentPoints:=(@discernmentPoints + 5) WHERE gloveCode = _gloveCode;	
	              UPDATE Users SET streaks = 0 WHERE gloveCode = _gloveCode;		   
		   END IF;
		ELSE
		   IF @streaks = 3 THEN
		      UPDATE Users SET discernmentPoints = 100 WHERE gloveCode = _gloveCode;
		      UPDATE Users SET streaks = 0 WHERE gloveCode = _gloveCode;				   
		   END IF;		   
	        END IF;
	   ELSE
	   	IF @operation = '*' OR @operation = '/' AND @abstractPoints > 2 THEN
	           UPDATE Users SET abstractPoints = @abstractPoints:=(@abstractPoints - 2) WHERE gloveCode = _gloveCode;
	        ELSEIF @operation = '*' OR @operation = '/' THEN
	           UPDATE Users SET abstractPoints = 0 WHERE gloveCode = _gloveCode;
	        END IF;
	        
	        IF @operation = '+' OR @operation = '-' AND @memoryPoints > 5 THEN
	           UPDATE Users SET memoryPoints = @memoryPoints:=(@memoryPoints - 5) WHERE gloveCode = _gloveCode;
	        ELSEIF @operation = '+' OR @operation = '-' THEN
	           UPDATE Users SET memoryPoints = 0 WHERE gloveCode = _gloveCode;
	        END IF;
	        
		IF @calculusPoints > 3 THEN
		   UPDATE Users SET calculusPoints = @calculusPoints:=(@calculusPoints - 3) WHERE gloveCode = _gloveCode;
		ELSE
		   UPDATE Users SET calculusPoints = 0 WHERE gloveCode = _gloveCode;	
		END IF;		
	        UPDATE Users SET negativeStreaks = negativeStreaks + 1 WHERE gloveCode = _gloveCode;
	        UPDATE Users SET streaks = 0 WHERE gloveCode = _gloveCode;
	        
	        IF @discernmentPoints > 5 THEN 
		   IF @negativeStreaks = 3 THEN
		      UPDATE Users SET discernmentPoints = @discernmentPoints:=(@discernmentPoints - 5) WHERE gloveCode = _gloveCode;	
	              UPDATE Users SET negativeSteaks = 0 WHERE gloveCode = _gloveCode;		   
		   END IF;
		ELSE
		   IF @negativeStreaks = 3 THEN
		      UPDATE Users SET discernmentPoints = 0 WHERE gloveCode = _gloveCode;
		      UPDATE Users SET negativeStreaks = 0 WHERE gloveCode = _gloveCode;				   
		   END IF;		   
	        END IF;	        
	   END IF;		      
	END IF;
 END//
DELIMITER ;

DELIMITER //
CREATE PROCEDURE SP_ShowModule(
  _idModule INT
)
BEGIN
	SET @sumaPoints = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND r.state = TRUE AND (o.operation = '+'));
	 
	SET @restaPoints = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND r.state = TRUE AND (o.operation = '-'));
	 
	SET @multiplicationPoints = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND r.state = TRUE AND (o.operation = '*'));
	 
	SET @divisionPoints = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND r.state = TRUE AND (o.operation = '/'));
	 
	SELECT @sumaPoints AS sumaPoints,@restaPoints AS restaPoints,@multiplicationPoints AS multiplicationPoints,@divisionPoints AS divisionPoints;
 END//
DELIMITER ;


DELIMITER //
CREATE PROCEDURE SP_ShowDateModule(
  _idModule INT
)
BEGIN
	SET @january = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND
	 r.state = TRUE AND MONTH(o.created_at) = 1 AND YEAR(o.created_at) = YEAR(CURDATE()));
	 
	 SET @february = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND
	 r.state = TRUE AND MONTH(o.created_at) = 2 AND YEAR(o.created_at) = YEAR(CURDATE()));
	 
	 SET @march = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND
	 r.state = TRUE AND MONTH(o.created_at) = 3 AND YEAR(o.created_at) = YEAR(CURDATE()));
	 
	 SET @april = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND
	 r.state = TRUE AND MONTH(o.created_at) = 4 AND YEAR(o.created_at) = YEAR(CURDATE()));
	 
	 SET @may = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND
	 r.state = TRUE AND MONTH(o.created_at) = 5 AND YEAR(o.created_at) = YEAR(CURDATE()));
	 
	 SET @june = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND
	 r.state = TRUE AND MONTH(o.created_at) = 6 AND YEAR(o.created_at) = YEAR(CURDATE()));
	 
	 SET @july = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND
	 r.state = TRUE AND MONTH(o.created_at) = 7 AND YEAR(o.created_at) = YEAR(CURDATE()));
	 
	 SET @august = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND
	 r.state = TRUE AND MONTH(o.created_at) = 8 AND YEAR(o.created_at) = YEAR(CURDATE()));
	 
	 SET @september = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND
	 r.state = TRUE AND MONTH(o.created_at) = 9 AND YEAR(o.created_at) = YEAR(CURDATE()));	
	
	 SET @october = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND
	 r.state = TRUE AND MONTH(o.created_at) = 10 AND YEAR(o.created_at) = YEAR(CURDATE()));
	 
	 SET @november = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND
	 r.state = TRUE AND MONTH(o.created_at) = 11 AND YEAR(o.created_at) = YEAR(CURDATE()));
	 
	 SET @december = (SELECT COUNT(*) FROM Operations o
	 INNER JOIN Responses r WHERE o.idOperation = r.idOperation AND o.idModule = _idModule AND
	 r.state = TRUE AND MONTH(o.created_at) = 12 AND YEAR(o.created_at) = YEAR(CURDATE()));	 
	 
	SELECT @january AS january,@february AS february,@march AS march,@april AS april,@may AS may,@june AS june,@july AS july,@august AS august,
	@september AS september,@october AS october,@november AS november,@december AS december;
 END//
DELIMITER ;


DELIMITER //
CREATE PROCEDURE SP_ShowUsersModule(
  _idModule INT
)
BEGIN
	(SELECT u.gloveCode, u.calculusPoints AS calculusPoints, u.abstractPoints AS abstractPoints, u.memoryPoints AS memoryPoints,
	 u.discernmentPoints AS discernmentPoints FROM Users u 
	 INNER JOIN UsersModule um WHERE um.idModule = _idModule AND um.gloveCode = u.gloveCode);
 END//
DELIMITER ;

DELIMITER //
CREATE PROCEDURE SP_ShowModulePoints(
  _idModule INT
)
BEGIN
	(SELECT SUM(u.calculusPoints) AS calculusPoints, SUM(u.abstractPoints) AS abstractPoints, SUM(u.memoryPoints) AS memoryPoints,
	 SUM(u.discernmentPoints) AS discernmentPoints FROM Users u 
	 INNER JOIN UsersModule um WHERE um.idModule = _idModule AND um.gloveCode = u.gloveCode);
 END//
DELIMITER ;

DELIMITER //
CREATE PROCEDURE SP_ShowUsersPoints(
  _idModule INT
)
BEGIN
	(SELECT u.gloveCode, u.calculusPoints + u.abstractPoints + u.memoryPoints + u.discernmentPoints AS points FROM Users u 
	 INNER JOIN UsersModule um WHERE um.idModule = _idModule AND um.gloveCode = u.gloveCode);
 END//
DELIMITER ;

INSERT INTO Modules(module) VALUES('Estuardo');

CALL SP_CreateUser('1234');
CALL SP_CreateUser('5678');

INSERT INTO UsersModule(gloveCode,idModule) VALUES ('1234',1);
INSERT INTO UsersModule(gloveCode,idModule) VALUES ('5678',1);

-- CALL SP_CreateQuestion(1,NULL,5,'*',5,25,NULL,'Preguntar','all');
-- CALL SP_CreateQuestion(1,NULL,10,'/',5,2,NULL,'Preguntar','all');
-- CALL SP_CreateQuestion(1,NULL,5,'+',5,10,NULL,'Preguntar','all');
-- CALL SP_CreateQuestion(1,NULL,5,'-',5,0,NULL,'Preguntar','all');
-- CALL SP_CreateQuestion(1,NULL,10,'-',4,6,NULL,'Preguntar','all');
-- 
-- CALL SP_ResponseQuestion(1,'1234',25);
-- CALL SP_ResponseQuestion(2,'1234',2);
-- CALL SP_ResponseQuestion(3,'1234',10);
-- CALL SP_ResponseQuestion(4,'1234',0);
-- CALL SP_ResponseQuestion(5,'1234',7);
-- 
-- CALL SP_ResponseQuestion(1,'5678',20);
-- CALL SP_ResponseQuestion(2,'5678',2);
-- CALL SP_ResponseQuestion(3,'5678',5);
-- CALL SP_ResponseQuestion(4,'5678',0);
-- CALL SP_ResponseQuestion(5,'5678',6);
-- 
-- CALL SP_ShowUsersModule(1);
-- CALL SP_ShowModule(1);
-- CALL SP_ShowUsersPoints(1);
-- CALL SP_ShowModulePoints(1);
-- CALL SP_ShowDateModule(1);
-- 
-- SELECT * FROM Users;
-- SELECT * FROM Operations;