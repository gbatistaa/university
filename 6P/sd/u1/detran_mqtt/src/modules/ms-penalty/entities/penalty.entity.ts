import { CommonEntity } from 'src/modules/common/entities/common.entity';
import { Conductor } from 'src/modules/ms-conductor/entities/conductor.entity';
import { Column, Entity, ManyToOne } from 'typeorm';

@Entity()
export class Penalty extends CommonEntity {
  @Column({ type: 'text', nullable: false })
  description: string;

  @Column({ type: 'float', nullable: false })
  pontuation: number;

  @Column({ type: 'text', nullable: false })
  conductorCpf: string;

  @ManyToOne(() => Conductor, (conductor) => conductor.penalties)
  conductor: Conductor;
}
