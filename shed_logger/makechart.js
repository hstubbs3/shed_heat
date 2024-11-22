
      //time,"SHED","HEATSINK","SAND","RESERVOIR","ROOF","AMBIENT","LOOP PUMP","XFER PUMP","HEAT FAN","110V AC HEAT","",
      function makeChart(players) {
        var timestamps = players.map(function (d) {
          return d.time;
        });
        var footwarm = players.map(function (d) {
          return d.FOOTWARM;
        });
        var shed = players.map(function (d) {
          return d.SHED;
        });

        var floortemp = players.map(function (d) {
          return d.FLOOR;
        });

        var heatera = players.map(function (d) {
          return d.HEATERA;
        });

        var heaterb = players.map(function (d) {
          return d.HEATERB;
        });

        var sanda = players.map(function (d) {
          return d.SANDA;
        });

        var sandb = players.map(function (d) {
          return d.SANDB;
        });

        var reservoir = players.map(function (d) {
          return d.RESERVOIR;
        });

        var roof = players.map(function (d) {
          return d.ROOF;
        });

        var ambient = players.map(function (d) {
          return d.AMBIENT;
        });

       var loop_pump = players.map(function (d) {
          return d.LOOP_PUMP*0.2-1.1;
        });
       var xfera_pump = players.map(function (d) {
          return d.XFERA_PUMP*0.3-1.1;
        });

       var xferb_pump = players.map(function (d) {
          return d.XFERB_PUMP*0.4-1.1;
        });

       var heater_a_fan = players.map(function (d) {
          return d.HEATER_A_FAN*0.5-1.1;
        });
       var heater_a_elements = players.map(function (d) {
          return d.HEATER_A_ELEMENTS*0.6-1.1;
        });

       var heater_b_fans = players.map(function (d) {
          return d.HEATER_B_FANS*0.7-1.1;
        });
       var heater_b_elements = players.map(function (d) {
          return d.HEATER_B_ELEMENTS*0.8-1.1;
        });

       var foot_warmer = players.map(function (d) {
          return d.FOOT_WARMER*0.9-1.1;
        });

       var xfera_heat = players.map(function (d) {
          return d.XFERA_HEAT*1.0-1.1;
        }); 
       var amps = players.map(function (d) {
          return d.Watts/120.0;
       });

       var shed_hum = players.map(function (d) {
          return d.SHED_HUM;
       });

       var ambient_hum = players.map(function (d) {
          return d.AMBIENT_HUM;
       });

       var light = players.map(function (d) {
          return d.light;
       });

        var chart = new Chart("myChart", {
          
          data: {
            labels: timestamps,
            datasets: [

              {type: "line",
                data: loop_pump, label: "LOOP_PUMP", backgroundColor: "#000088", yAxisID: 'y1', pointRadius:1,
                borderColor: "#000088", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: xfera_pump, label: "XFERA_PUMP", backgroundColor: "#008888", yAxisID: 'y1',pointRadius:1,//  fill : false
                borderColor: "#008888", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: xferb_pump, label: "XFERB_PUMP", backgroundColor: "#004444", yAxisID: 'y1',pointRadius:1,//  fill : false
                borderColor: "#004444", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: heater_a_fan, label: "HEATERA_FAN", backgroundColor: "#FF8888", yAxisID: 'y1',pointRadius:1,
                borderColor: "#FF8888", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: heater_a_elements, label: "HEATER_A_ELEMENTS", backgroundColor: "#FF8800", yAxisID: 'y1',pointRadius:1,
                borderColor: "#FF8800", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: heater_b_fans, label: "HEATER_B_FANS", backgroundColor: "#FF00FF", yAxisID: 'y1',pointRadius:1,
                borderColor: "#FF00FF", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: heater_b_elements, label: "HEATER_B_ELEMENTS", backgroundColor: "#FF0088", yAxisID: 'y1',pointRadius:1,
                borderColor: "#880088", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },

              {type: "line",
                data: foot_warmer, label: "FOOT_WARMER", backgroundColor: "#FF0000", yAxisID: 'y1',pointRadius:1,
                borderColor: "#FF0000", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: xfera_heat, label: "XFERA_HEAT", backgroundColor: "#CC6600", yAxisID: 'y1',pointRadius:1,
                borderColor: "#FF0000", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },

              {type: "line",
                data: amps, label: "AMPS (TOTAL/SHED)", backgroundColor: "#888844", yAxisID: 'y1',pointRadius:5,
                borderColor: "#888844", borderWidth: 4,pointStyle: 'cross',

              },


              {type: "line",
                data: footwarm, label: "FOOTWARM", borderColor: "#FF8888", fill : false, yAxisID: 'y',pointRadius:1,
              },
              {type: "line",
                data: shed, label: "SHED", borderColor: "#000000", fill : false, yAxisID: 'y',pointRadius:1,
              },
              {type: "line",
                data: floortemp, label: "FLOOR", borderColor: "#444444", fill : false, yAxisID: 'y',pointRadius:1,
              },

              {type: "line",
                data: heatera, label: "HEATERA", borderColor: "#FF0000", fill : false, yAxisID: 'y',pointRadius:1,
              },
              {type: "line",
                data: heaterb, label: "HEATERB", borderColor: "#FF0088", fill : false, yAxisID: 'y',pointRadius:1,
              },
              {type: "line",
                data: sanda, label: "SANDA", borderColor: "#888800", fill : false, yAxisID: 'y',pointRadius:1,
              },
              {type: "line",
                data: sandb, label: "SANDB", borderColor: "#666600", fill : false, yAxisID: 'y',pointRadius:1,
              },

              {type: "line",
                data: reservoir, label: "RESERVOIR", borderColor: "#008888", fill : false, yAxisID: 'y',pointRadius:1,
              },
              {type: "line",
                data: roof, label: "ROOF", borderColor: "#882200", fill : false, yAxisID: 'y',pointRadius:1,
              },
              {type: "line",
                data: ambient, label: "AMBIENT", borderColor: "#888888", yAxisID: 'y',pointRadius:1,
              },
              {type: "line",
                data: ambient_hum, label: "AMBIENT HUMIDITY", borderColor: "#8888CC", yAxisID: 'y',pointRadius:3,pointStyle:'triangle'
              },
              {type: "line",
                data: shed_hum, label: "SHED HUMIDITY", borderColor: "#000044", yAxisID: 'y',pointRadius:3,pointStyle:'triangle'
              },
              {type: "line",
                data: light, label: "LIGHT LEVEL", borderColor: "#CCCC00", yAxisID: 'light',pointRadius:8,pointStyle:'star', 
                borderWidth: 4,
              },


            ]
          },
                      options: {
              scales: {
                y: {
                  position: 'left',
                  min: 0,
                  max: 120,
                  grid: {
                    lineWidth: 2,
                    color: "#666666",
                  }
                },
                y1: {
                  position: 'right',
                  min: -1,
                  max: 11,
                  grid: {
                    lineWidth: 2,
                    color: "#444444"
                  },
                },
                light: {
                  position: 'right',
                  min: -67,
                  max: 733,
                }
              },
            },

        });



      }
