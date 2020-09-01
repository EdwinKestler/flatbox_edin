import { Component, OnInit, ViewEncapsulation } from '@angular/core';
import { NgbModal, ModalDismissReasons } from '@ng-bootstrap/ng-bootstrap';
import { NetworkService } from '../../services/network.service';

@Component({
  selector: 'app-network',
  templateUrl: './network.component.html',
  styleUrls: ['./network.component.css'],
  encapsulation: ViewEncapsulation.None
})
export class NetworkComponent implements OnInit {

  closeResult: string;

  networks:any = [];

  selected:any;
  password:string;
  connected:boolean = false;

  constructor(private modalService: NgbModal,private networkService:NetworkService) { }

  ngOnInit(): void {
    this.getNetworks();
    this.currentConnection();
  }

  getNetworks(){
    this.networks = [];
    this.networkService.getNetworks().then((results) => {
      this.networks = results;
    })
  }

  currentConnection(){
    this.networkService.currentConnection().then((results) => {
      if(results != null){
        this.connected = true;
      }
    })
  }

  connectWifi(){
    let network = {
      ssid: this.selected.ssid,
      password: this.password
    }

    this.networkService.connectWifi(network).then((result) => {
      console.log(result);
    })
  }

  disconnectWifi(){
    this.networkService.disconnectWifi().then((results) => {
      console.log(results);
      this.connected = false;
      this.getNetworks();
    });
  }

  onSelect(item:any){
    this.selected = item;
  }

  open(content: any) {
    this.modalService.open(content, { ariaLabelledBy: 'modal-basic-title' }).result.then((result) => {
      this.closeResult = `Closed with: ${result}`;
    }, (reason) => {
      this.closeResult = `Dismissed ${this.getDismissReason(reason)}`;
    });
  }

  private getDismissReason(reason: any): string {
    if (reason === ModalDismissReasons.ESC) {
      return 'by pressing ESC';
    } else if (reason === ModalDismissReasons.BACKDROP_CLICK) {
      return 'by clicking on a backdrop';
    } else {
      return `with: ${reason}`;
    }
  }

}
