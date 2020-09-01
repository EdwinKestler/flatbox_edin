import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { HomeComponent } from './components/home/home.component';
import { StatisticsComponent } from './components/statistics/statistics.component';
import { DetailsComponent } from './components/details/details.component';
import { NetworkComponent } from './components/network/network.component';

const routes: Routes = [
  { path: '', component: HomeComponent },
  { path: "home", component: HomeComponent},
  { path: "estadisticas", component: StatisticsComponent},
  { path: "details/:idOperation", component: DetailsComponent},
  { path: "network", component: NetworkComponent},
  { path: '**', pathMatch: 'full', redirectTo: 'home'}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
