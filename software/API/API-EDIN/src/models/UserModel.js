const {pool} = require('../db/connection');
const userModel = {}

userModel.getAll = async() => {
    const connection = await pool.getConnection();
    try{
        const data = await connection.query('SELECT * FROM Users');
        return data[0];
    }catch(ex){
        console.log(ex);
    }finally{
        connection.release();
    }
}

userModel.createUser = async(newUser) => {
    const connection = await pool.getConnection();
    try{
        const sql = "CALL SP_CreateUser(?)"
        const data = await connection.query(sql,[newUser.gloveCode]);
        return data[0];
    }catch(ex){
        console.log(ex);
    }finally{
        connection.release();
    } 
}

module.exports = userModel;