let config;

if (process.env.NODE_ENV === "production") {
  config = {
    $url: "",
  };
} else {
  config = {
    $url: "http://192.168.0.196",
  };
}

export { config }