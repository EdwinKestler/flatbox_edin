import { Component, OnInit,ViewEncapsulation } from '@angular/core';
import { StatisticsService } from '../../services/statistics.service';

@Component({
  selector: 'app-statistics',
  templateUrl: './statistics.component.html',
  styleUrls: ['./statistics.component.css'],
  encapsulation: ViewEncapsulation.None
})
export class StatisticsComponent implements OnInit {
  single:any[];
  single2: any[];
  single3: any[];
  single4: any[];

  viewPie: any[] = [320, 250];
  viewV: any[] = [280, 305];
  viewH: any[] = [300, 305];
  viewGrid: any[] = [1200, 200];

  // options bar
  showXAxis: boolean = true;
  showYAxis: boolean = true;
  gradient: boolean = false;
  showLegend: boolean = false;
  showXAxisLabel: boolean = true;
  yAxisLabel: string = 'Habilidades';
  showYAxisLabel: boolean = true;
  xAxisLabel: string = 'Puntos Obtenidos';

  // options pie
  gradientPie: boolean = true;
  showLegendPie: boolean = false;
  showLabels: boolean = true;
  isDoughnut: boolean = false;
  legendPosition: string = 'below';


  colorScheme = {
    domain: ['#59bbce', '#527384', '#4e85ce', '#285361', '#50dfff', '#526392']
  };

  constructor(private statisticsService:StatisticsService) { 
  }

  ngOnInit(): void {
    this.getModule(1);
    this.getDateModule(1);
    this.getUsersPoints(1);
    this.getModulePoints(1);
  }

  async getModule(idModule: number){
    this.single2 = [];
    await this.statisticsService.getModule(idModule).then((results:any) => {
      let operations = [
        {
          name: 'Suma',
          value: parseInt(results[0].sumaPoints)
        },
        {
          name: 'Resta',
          value: parseInt(results[0].restaPoints)       
        },
        {
          name: 'Multiplicación',
          value: parseInt(results[0].multiplicationPoints)  
        },
        {
          name: 'División',
          value: parseInt(results[0].divisionPoints) 
        }
      ];

      this.single2 = operations;
    })
  }

  async getUsersPoints(idModule: number){
    await this.statisticsService.getUsersPoints(idModule).then((results: any) => {
      let users = []
      results.forEach(element => {
        let user = {
          name: 'Glove Code: ' + element.gloveCode,
          value: element.points
        }
        users.push(user);
      });

      this.single3 = users;
    });
  }

  async getModulePoints(idModule: number){
    await this.statisticsService.getModulePoints(idModule).then((results:any) => {

      console.log(results);

      let points = [
        {
          name: 'Cálculo',
          value: parseInt(results[0].calculusPoints)
        },
        {
          name: 'Abstracto',
          value: parseInt(results[0].abstractPoints)       
        },
        {
          name: 'Memoria',
          value: parseInt(results[0].memoryPoints)  
        },
        {
          name: 'Discernimiento',
          value: parseInt(results[0].discernmentPoints) 
        }
      ];

      this.single4 = points;
    });
  }

  async getDateModule(idModule:number){
    await this.statisticsService.getDateModule(idModule).then((results:any) => {
      let points = [
        { name: 'Enero',value: parseInt(results[0].january)},
        { name: 'Febrero',value: parseInt(results[0].february)},
        { name: 'Marzo',value: parseInt(results[0].march)},
        { name: 'Abril',value: parseInt(results[0].april)},
        { name: 'Mayo',value: parseInt(results[0].may)},
        { name: 'Junio',value: parseInt(results[0].june)},
        { name: 'Julio',value: parseInt(results[0].july)},
        { name: 'Agosto',value: parseInt(results[0].august)},
        { name: 'Septiembre',value: parseInt(results[0].september)},
        { name: 'Octubre',value: parseInt(results[0].october)},
        { name: 'Noviembre',value: parseInt(results[0].november)},
        { name: 'Diciembre',value: parseInt(results[0].december)},
      ];

      this.single = points;
    });
  }

  onSelect(data): void {
    // console.log('Item clicked', JSON.parse(JSON.stringify(data)));
  }

  onActivate(data): void {
    // console.log('Activate', JSON.parse(JSON.stringify(data)));
  }

  onDeactivate(data): void {
    // console.log('Deactivate', JSON.parse(JSON.stringify(data)));
  }
}
