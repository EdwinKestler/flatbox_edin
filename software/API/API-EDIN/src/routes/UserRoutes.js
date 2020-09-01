const User = require('../models/UserModel');

module.exports = (app) => {
    app.get('/users',async (req,res) => {
        try{
            const results = await User.getAll();
            if (typeof results !== 'undefined' && results.length > 0) {
                res.status(200).json(results);
            } else {
                res.status(200).json(results);
            }
        }catch(error){
            console.log(error);
            res.status(500).json(error);
        }
    });

    app.post("/user", async (req,res) => {
        let newUser = {
            gloveCode: req.body.gloveCode,
        }

        try{
            const results = await User.createUser(newUser);
            if (typeof results !== 'undefined' && results.length > 0) {
                res.status(200).json({message: 'Usuario Creado'});
            } else {
                res.status(200).json({message: 'Usuario Creado'});
            }
        }catch(error){
            console.log(error);
            res.status(500).json(error);
        }
    });

}