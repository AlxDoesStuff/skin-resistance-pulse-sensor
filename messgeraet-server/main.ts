Deno.serve({ port: 8000, hostname: "0.0.0.0" }, (_req) => {
  let requestURL = _req.url; //Store request URL
  console.log(requestURL); //Write request URL to console
  return new Response("OK"); //Return OK as response
});

