import {
  Body,
  Controller,
  Get,
  Param,
  Patch,
  Post,
  Query,
} from '@nestjs/common';
import { GatewayService } from './gateway.service';
import { CreateVehicleDto } from '../ms-vehicle/dto/create-vehicle.dto';
import { CreateConductorDto } from '../ms-conductor/dto/create-conductor.dto';
import { UpdateConductorDto } from '../ms-conductor/dto/update-conductor.dto';
import { CreatePenaltyDto } from '../ms-penalty/dto/create-penalty.dto';

@Controller('gateway')
export class GatewayController {
  constructor(private readonly gatewayService: GatewayService) {}

  @Get('penalty/vehiclePenalties/:sign')
  getVehiclePenalties(
    @Param('sign') sign: string,
    @Query('year') year: string,
  ) {
    return this.gatewayService.getVehiclePenalties({ sign, year });
  }

  @Get('vehicle/signedThisYear/:year')
  getVehiclesSignedThisYear(@Param('year') year: string) {
    return this.gatewayService.getVehiclesSignedThisYear(year);
  }

  @Post('vehicle/signup')
  async signupVehicle(@Body() createVehicleDto: CreateVehicleDto) {
    return await this.gatewayService.signupVehicle(createVehicleDto);
  }

  @Post('conductor/signup')
  signupConductor(@Body() createConductorDto: CreateConductorDto) {
    return this.gatewayService.signupConductor(createConductorDto);
  }

  @Post('penalty/launch')
  launchPenalty(@Body() createPenaltyDto: CreatePenaltyDto) {
    return this.gatewayService.launchPenalty(createPenaltyDto);
  }

  @Patch('vehicle/updateConductor/:sign')
  updateVehicleConductor(
    @Body() updateVehicleDto: UpdateConductorDto,
    @Param('sign') sign: string,
  ) {
    return this.gatewayService.updateVehicleConductor({
      sign,
      updateVehicleDto,
    });
  }
}
