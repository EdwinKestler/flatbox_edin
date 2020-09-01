import { environment } from './../../environments/environment';
import { Injectable } from '@angular/core';
import { HttpHeaders, HttpClient } from '@angular/common/http';

@Injectable({
  providedIn: 'root'
})
export class NetworkService {

  url = environment.apiUrl;
  headers = new HttpHeaders();

  constructor(private http: HttpClient) { }

  getNetworks() {
    return this.http.get(this.url + 'wifi', { headers: this.setHeaders() }).toPromise();
  }

  connectWifi(data:any) {
    return this.http.post(this.url + 'wifi', data, { headers: this.setHeaders() }).toPromise();
  }

  disconnectWifi(){
    return this.http.get(this.url + 'disconnect', { headers: this.setHeaders() }).toPromise();
  }

  currentConnection(){
    return this.http.get(this.url + 'connection', { headers: this.setHeaders() }).toPromise();
  }

  setHeaders() {
    this.headers.append('Content-Type', 'application/json');
    return this.headers;
  }
}
