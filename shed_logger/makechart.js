
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
          return d.LOOP_PUMP*0.125-1.0;
        });
       var xfera_pump = players.map(function (d) {
          return d.XFERA_PUMP*0.25-1.0;
        });

       var xferb_pump = players.map(function (d) {
          return d.XFERB_PUMP*0.375-1.0;
        });

       var heater_a_fan = players.map(function (d) {
          return d.HEATER_A_FAN*0.5-1.0;
        });
       var heater_a_elements = players.map(function (d) {
          return d.HEATER_A_ELEMENTS*0.625-1.0;
        });

       var heater_b_fans = players.map(function (d) {
          return d.HEATER_B_FANS*0.75-1.0;
        });
       var heater_b_elements = players.map(function (d) {
          return d.HEATER_B_ELEMENTS*0.875-1.0;
        });

       var foot_warmer = players.map(function (d) {
          return d.FOOT_WARMER*1.0-1.0;
        });

       var amps = players.map(function (d) {
          return d.Watts/120.0;
       });

        var chart = new Chart("myChart", {
          
          data: {
            labels: timestamps,
            datasets: [

              {type: "line",
                data: loop_pump, label: "LOOP_PUMP", backgroundColor: "#000088", yAxisID: 'y1', 
                borderColor: "#000088", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: xfera_pump, label: "XFERA_PUMP", backgroundColor: "#008888", yAxisID: 'y1',//  fill : false
                borderColor: "#008888", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: xferb_pump, label: "XFERB_PUMP", backgroundColor: "#004444", yAxisID: 'y1',//  fill : false
                borderColor: "#004444", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: heater_a_fan, label: "HEATERA_FAN", backgroundColor: "#FF8888", yAxisID: 'y1',//  fill : false
                borderColor: "#FF8888", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: heater_a_elements, label: "HEATER_A_ELEMENTS", backgroundColor: "#FF8800", yAxisID: 'y1',// fill : false
                borderColor: "#FF8800", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: heater_b_fans, label: "HEATER_B_FANS", backgroundColor: "#FF00FF", yAxisID: 'y1',//  fill : false
                borderColor: "#FF00FF", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },
              {type: "line",
                data: heater_b_elements, label: "HEATER_B_ELEMENTS", backgroundColor: "#FF0088", yAxisID: 'y1',// fill : false
                borderColor: "#880088", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },

              {type: "line",
                data: foot_warmer, label: "FOOT_WARMER", backgroundColor: "#FF0000", yAxisID: 'y1',//  fill : false
                borderColor: "#FF0000", pointStyle: 'rect', borderWidth: 2, stepped: true,
              },

              {type: "line",
                data: amps, label: "AMPS (TOTAL/SHED)", backgroundColor: "#888844", yAxisID: 'y1',//  fill : false
                borderColor: "#888844",

              },


              {type: "line",
                data: footwarm, label: "FOOTWARM", borderColor: "#FF8888", fill : false, yAxisID: 'y',
              },
              {type: "line",
                data: shed, label: "SHED", borderColor: "#000000", fill : false, yAxisID: 'y',
              },
              {type: "line",
                data: heatera, label: "HEATERA", borderColor: "#FF0000", fill : false, yAxisID: 'y',
              },
              {type: "line",
                data: heaterb, label: "HEATERB", borderColor: "#FF0088", fill : false, yAxisID: 'y',
              },
              {type: "line",
                data: sanda, label: "SANDA", borderColor: "#888800", fill : false, yAxisID: 'y',
              },
              {type: "line",
                data: sandb, label: "SANDB", borderColor: "#8888FF", fill : false, yAxisID: 'y',
              },

              {type: "line",
                data: reservoir, label: "RESERVOIR", borderColor: "#008888", fill : false, yAxisID: 'y',
              },
              {type: "line",
                data: roof, label: "ROOF", borderColor: "#AA4422", fill : false, yAxisID: 'y',
              },
              {type: "line",
                data: ambient, label: "AMBIENT", borderColor: "#888888", yAxisID: 'y',
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
                  }
                }
              },
            },

        });



      }
