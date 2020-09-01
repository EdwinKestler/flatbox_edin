import { Component, OnInit, ViewEncapsulation  } from '@angular/core';
import { ActivatedRoute } from '@angular/router';
import { OperationsService } from '../../services/operations.service';

@Component({
  selector: 'app-details',
  templateUrl: './details.component.html',
  styleUrls: ['./details.component.css'],
  encapsulation: ViewEncapsulation.None
})
export class DetailsComponent implements OnInit {

  private sub: any;
  idOperation:number;
  operation = []
  responses = []

  constructor(private route:ActivatedRoute, private operationService:OperationsService) { }

  ngOnInit(): void {
    this.sub = this.route.params.subscribe(params => {
      this.idOperation = params['idOperation'];
      this.getOperation();
      this.getResponses();
    });
  }

  getResponses(){
    this.operationService.getResponses(this.idOperation).then((results:any) => {
      console.log(results);
      this.responses = results;
    });
  }

  getOperation(){
    this.operationService.getOperation(this.idOperation).then((results:any) => {
      this.operation = results;
    });
  }

}
