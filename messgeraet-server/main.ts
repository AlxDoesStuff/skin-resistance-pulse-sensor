Deno.serve({ port: 8000, hostname: "0.0.0.0" }, (_req) => {
  let requestURL = _req.url; //Store request URL
  let URLSplit = requestURL.split("/");
  //Unterscheide zwischen Verbindungsankündigung, Daten und Müll
  if(URLSplit[3]){
    if(URLSplit[3] == "connected" && URLSplit.length == 6){
      let connectTimeParsed = parseInt(URLSplit[4]); //Die Zeit, seitdem der Verbundene ESP läuft in ms (int)
      let connectIDParsed = parseInt(URLSplit[5]); //Die ID des verbundenen ESPs in ms (int)
      console.log("New ESP connected. ID: " + connectIDParsed + ", running for: " + connectTimeParsed);
    }else if(URLSplit[3] == "data" && URLSplit.length == 8){
      let dataTimeParsed = parseInt(URLSplit[6]); //Die Zeit, seitdem der Verbundene ESP läuft in ms (int)
      let dataIDParsed = parseInt(URLSplit[7]); //Die ID des ESPs, der die Daten in ms (int)
      let pulseParsed = parseInt(URLSplit[4]); 
      let skinResistanceParsed = parseInt(URLSplit[5]); 
      console.log("Data recieved. ID: " + dataIDParsed + ", data at time: " + dataTimeParsed + ". Pulse: "+ pulseParsed + " Skin Resistance: "+skinResistanceParsed);
    }else{  
      console.log("Incorrect URL: "+requestURL); 
    }
    
  }  return new Response("OK"); //Auf alle requests OK als response 
});

