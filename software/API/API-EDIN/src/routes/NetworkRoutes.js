const wifi = require('node-wifi');

wifi.init({
  iface: null
});

module.exports = (app) => {
  app.get('/wifi',async (req,res) => {
    try{
      wifi.scan().then((networks) => {
        res.status(200).json(networks);
      }).catch((error) => {
        res.status(500).json(error);
      })
    }catch(error){
      console.log(error);
      res.status(500).json(error);
    }
  });

  app.get('/connection',async (req,res) => {
    try{
      wifi.getCurrentConnections().then((currentConnection) => {
        res.status(200).json(currentConnection);
      }).catch((error) => {
        res.status(500).json(error);
      })
    }catch(error){
      console.log(error);
      res.status(500).json(error);
    }
  });

  app.get('/disconnect',async (req,res) => {
    try{
      wifi.disconnect().then(() => {
        res.status(200).json({message: 'Disconnected'})
      }).catch((error) => {
        res.status(500).json(error);
      });
    }catch(error){
      console.log(error);
      res.status(500).json(error);
    }
  });

  app.post("/wifi", async (req,res) => {
    let network = {
        ssid: req.body.ssid,
        password: req.body.password
    }

    try{
      wifi.connect(network).then(() => {
        res.status(200).json({message: 'Connected'});
      }).catch((error) =>{
        res.status(500).json(error);
      })
    }catch(error){
      console.log(error);
      res.status(500).json(error);
    }
  });

}