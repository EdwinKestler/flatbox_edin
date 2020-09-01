import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { environment } from 'src/environments/environment';

@Injectable({
  providedIn: 'root'
})
export class OperationsService {

  url = environment.apiUrl;
  url2 = "http://192.168.1.9:1880/";
  headers = new HttpHeaders();

  constructor(private http: HttpClient) { }

  getResponses(idOperation: any){
    return this.http.get(this.url + 'responses/' + idOperation, { headers: this.setHeaders() }).toPromise();
  }

  getOperation(idOperation: any){
    return this.http.get(this.url + 'operation/' + idOperation, { headers: this.setHeaders() }).toPromise();
  }

  postOperationDB(data:any) {
    return this.http.post(this.url + 'operation', data, { headers: this.setHeaders() }).toPromise();
  }

  postQuestionMQTT(data:any){
    return this.http.post(this.url2 + 'preguntas', data, { headers: this.setHeaders() }).toPromise();
  }

  getAll() {
    return this.http.get(this.url + 'operations', { headers: this.setHeaders() }).toPromise();
  }

  getUsers(){
    return this.http.get(this.url + 'users', { headers: this.setHeaders() }).toPromise();
  }

  setHeaders() {
    this.headers.append('Content-Type', 'application/json');
    return this.headers;
  }
}
