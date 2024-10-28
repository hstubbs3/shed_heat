
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
          return d.LOOP_PUMP;
        });
       var xfera_pump = players.map(function (d) {
          return d.XFERA_PUMP;
        });

       var xferb_pump = players.map(function (d) {
          return d.XFERB_PUMP;
        });

       var heater_a_fan = players.map(function (d) {
          return d.HEATER_A_FAN;
        });
       var heater_a_elements = players.map(function (d) {
          return d.HEATER_A_ELEMENTS;
        });

       var heater_b_fans = players.map(function (d) {
          return d.HEATER_B_FANS;
        });
       var heater_b_elements = players.map(function (d) {
          return d.HEATER_B_ELEMENTS;
        });

       var foot_warmer = players.map(function (d) {
          return d.FOOT_WARMER;
        });

       var amps = players.map(function (d) {
          return d.Watts/120.0;
       });

        var chart = new Chart("myChart", {
          
          data: {
            labels: timestamps,
            datasets: [
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

              {type: "bar",
                data: loop_pump, label: "LOOP_PUMP", backgroundColor: "#000088", yAxisID: 'y1',// fill : false
              },
              {type: "bar",
                data: xfera_pump, label: "XFERA_PUMP", backgroundColor: "#008888", yAxisID: 'y1',//  fill : false
              },
              {type: "bar",
                data: xferb_pump, label: "XFERB_PUMP", backgroundColor: "#004444", yAxisID: 'y1',//  fill : false
              },
              {type: "bar",
                data: heater_a_fan, label: "HEATERA_FAN", backgroundColor: "#FF8888", yAxisID: 'y1',//  fill : false
              },
              {type: "bar",
                data: heater_a_elements, label: "HEATER_A_ELEMENTS", backgroundColor: "#FF8800", yAxisID: 'y1',// fill : false
              },
              {type: "bar",
                data: heater_b_fans, label: "HEATER_B_FANS", backgroundColor: "#FF00FF", yAxisID: 'y1',//  fill : false
              },
              {type: "bar",
                data: heater_b_elements, label: "HEATER_B_ELEMENTS", backgroundColor: "#FF0088", yAxisID: 'y1',// fill : false
              },

              {type: "bar",
                data: foot_warmer, label: "FOOT_WARMER", backgroundColor: "#FF0000", yAxisID: 'y1',//  fill : false
              },

              {type: "line",
                data: amps, label: "AMPS (TOTAL/SHED)", backgroundColor: "#888844", yAxisID: 'y1',//  fill : false

              },

            ]
          },
                      options: {
              scales: {
                y: {
                  position: 'left',
                  min: 20,
                  max: 100,
                },
                y1: {
                  position: 'right',
                  min: 0,
                  max: 8
                }
              }
            },

        });



      }
