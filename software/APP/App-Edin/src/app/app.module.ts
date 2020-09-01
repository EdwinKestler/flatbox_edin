import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';

//LIBRERIAS AGREGADAS
import { NgbModule } from '@ng-bootstrap/ng-bootstrap';
import { NgbPaginationModule, NgbAlertModule } from '@ng-bootstrap/ng-bootstrap';
import { AppRoutingModule } from './app-routing.module';
import { ReactiveFormsModule, FormsModule } from '@angular/forms';

//COMPONENTS
import { AppComponent } from './app.component';
import { HomeComponent } from './components/home/home.component';
import { NavbarComponent } from './components/navbar/navbar.component';

//SERVICES
import { OperationsService } from './services/operations.service';
import { HttpClientModule } from '@angular/common/http';
import { StatisticsComponent } from './components/statistics/statistics.component';
import { DetailsComponent } from './components/details/details.component';
import { NetworkComponent } from './components/network/network.component';

import { NgxChartsModule } from '@swimlane/ngx-charts';


@NgModule({
  declarations: [
    AppComponent,
    HomeComponent,
    NavbarComponent,
    StatisticsComponent,
    DetailsComponent,
    NetworkComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    ReactiveFormsModule,
    NgbModule,
    NgbPaginationModule,
    NgbAlertModule,
    FormsModule,
    HttpClientModule,
    NgxChartsModule,
    BrowserAnimationsModule
  ],
  providers: [
    OperationsService
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
