<?php
    enum state{
        case IDLE;
        case COMMENT;
        case HEADER;

        case DES_CHECK; 
        case SYM_CHECK; 
        case LAB_CHECK; 
        case VAR_CHECK; 
        case TYP_CHECK; 

        case ROW_END;

        case VAR;


        case OP_VS;
        case OP_VT;
        case OP_VSS;
        case OP_LSS;

        case ADD_AND;
        case I;
        case INT2CHAR;
        case IDIV;
        case M;
        case MUL;
        case MOVE;
        case C;
        case CREATEFRAME;
        case CALL;
        case CONCAT;
        case P;
        case PU;
        case PO;
        case P_FRAME;   // PUSHF a POPF jsou zkontrolovane, zbytek OP je stejny
        //case PUSHS;   z P volam SYM_CHECK
        //case POPS;    z P volam VAR_CHECK
        case J;
        //case JUMP;    z J volam LAB_CHECK
        case JUMPIFEQ;
        case JUMPIFNEQ;
        case D;
        case DEFVAR;
        case DPRINT;

        case R;
        case RETURN;
        case READ;

        case LABEL;
        //case LT;  possibly soucasti LABEL -> OP_VSS

        case GETCHAR;
        //case GT;  possibly soucasti GETCHAR -> OP_VSS

        case EXIT;
        //case EQ;  possibly soucasti EXIT -> OP_VSS

        
        case OR;

        case NOT;

        
        case S;
        case SUB;
        case STRI2INT;
        case STRLEN;
        case SETCHAR;

        case WRITE;

        case TYPE;

        case BREAK;
    }

    function error_func($err){
        echo "ERROR\n";
        exit;
    }

    $file = fopen('php://stdin', 'r');
    
    $state = state::IDLE;
    $main_ptr = 0;
    $side_ptr = 0;
    $header = false;
    $status = false;
    $called_from = state::IDLE;
    while (!feof($file)){
        $char = fgetc($file);
        switch ($state) {
            case state::IDLE:
                if (strcmp($char,"#") == 0){
                    $state = state::COMMENT;
                }
                elseif (strcmp($char,".") == 0){
                    if ($header == false){
                        $state = state::HEADER;
                    }
                    else {
                        error_func(1);
                    }                   
                }
                elseif ((strcmp($char,"\n") == 0)||(strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                    $state = state::IDLE;
                }
                elseif ($header == true) {
                    $main_ptr = 1;
                    $status = false;
                    if (strcmp($char, "A") == 0){$state = state::ADD_AND;}
                    elseif (strcmp($char, "S") == 0){$state = state::S;}
                    elseif (strcmp($char, "I") == 0){$state = state::I;}
                    elseif (strcmp($char, "M") == 0){$state = state::MUL;}
                    elseif (strcmp($char, "J") == 0){$state = state::J;}
                    elseif (strcmp($char, "C") == 0){$state = state::C;}
                    elseif (strcmp($char, "P") == 0){$state = state::P;}
                    elseif (strcmp($char, "D") == 0){$state = state::D;}
                    elseif (strcmp($char, "R") == 0){$state = state::R;}
                    elseif (strcmp($char, "L") == 0){$state = state::LABEL;}
                    elseif (strcmp($char, "G") == 0){$state = state::GETCHAR;}
                    elseif (strcmp($char, "E") == 0){$state = state::EXIT;}
                    elseif (strcmp($char, "O") == 0){$state = state::OR;}
                    elseif (strcmp($char, "N") == 0){$state = state::NOT;}
                    elseif (strcmp($char, "W") == 0){$state = state::WRITE;}
                    elseif (strcmp($char, "T") == 0){$state = state::TYPE;}
                    elseif (strcmp($char, "B") == 0){$state = state::BREAK;}

                    elseif (strcmp($char, "$") == 0){$state = state::VAR;}
                    else {
                        error_func(1);
                    }
                }
                else {
                    error_func(1);
                }
                break;

            case state::VAR:
                $state = state::IDLE;
                $main_ptr = 0;
                fgets($file);
                break;

            case state::COMMENT:
                if (strcmp($char,"\n") == 0){
                    $state = state::IDLE;
                    if ($header == true){
                        $status = true;
                    }
                }
                break;

            case state::HEADER:
                switch ($main_ptr) {
                    case 0:
                        if (strcasecmp($char,"i") != 0){
                            error_func(1);
                        }
                        break;
                    case 1:
                    case 2:
                        if (strcasecmp($char,"p") != 0){
                            error_func(1);
                        }
                        break;
                    case 3:
                        if (strcasecmp($char,"c") != 0){
                            error_func(1);
                        }
                        break;
                    case 4:
                        if (strcasecmp($char,"o") != 0){
                            error_func(1);
                        }
                        break;
                    case 5:
                        if (strcasecmp($char,"d") != 0){
                            error_func(1);
                        }
                        break;
                    case 6:
                        if (strcasecmp($char,"e") != 0){
                            error_func(1);
                        }
                        break;
                    case 7:
                        if (strcasecmp($char,"2") != 0){
                            error_func(1);
                        }
                        break;
                    case 8:
                        if (strcasecmp($char,"3") != 0){
                            error_func(1);
                        }
                        $header = true;
                        break;
                    default:
                        $main_ptr = -2;
                        $header = true;
                        if (strcmp($char,"#") == 0){
                            $state = state::COMMENT;
                            $main_ptr++;
                        }
                        elseif ((strcmp($char,"\n") == 0)||(strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $state = state::IDLE;
                            $main_ptr++;
                        }
                        else {
                            error_func(1);
                        }
                        break;
                }
                $main_ptr++;
                break;
            
            case state::OP_VS:
                switch($main_ptr){
                    case 0:
                    case 2: // alespon jeden bily znak
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $main_ptr++
                        }
                        else { error_func(1);}
                        break;
                    case 1: // bily znak nebo destination
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            continue;
                        }
                        elseif (strcmp($char, "\n") == 0) {
                            error_func(1);
                        }
                        else {
                            $main_ptr++;
                            $side_ptr = 0;
                            $state = state::DES_CHECK;
                            $state = state::OP_VS;
                        }
                        break;
                    case 3: // bily znak nebo source
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            continue;
                        }
                        elseif (strcmp($char, "\n") == 0) {
                            error_func(1);
                        }
                        else {
                            $main_ptr++;
                            $side_ptr = 0;
                            $state = state::SYM_CHECK;
                            $state = state::OP_VS;
                        }
                        break;
                    case 4: // radek konci
                        if (strcmp($char,"#") == 0){
                            $state = state::COMMENT;
                        }
                        elseif ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            continue;
                        }
                        elseif (strcmp($char,"\n") == 0){
                            $state = state::IDLE;
                            $status = true;
                        }
                        else {
                            error_func(1);
                        }
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::OP_VT:
                switch($main_ptr){
                    case 0:
                    case 2:
                    case 4: // alespon jeden bily znak
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $main_ptr++
                        }
                        else { error_func(1);}
                        break;
                    case 1: // bily znak nebo destination
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            continue;
                        }
                        elseif (strcmp($char, "\n") == 0) {
                            error_func(1);
                        }
                        else {
                            $main_ptr++;
                            $side_ptr = 0;
                            $state = state::VAR_CHECK;
                            $state = state::OP_VS;
                        }
                        break;
                    case 3: // bily znak nebo source
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            continue;
                        }
                        elseif (strcmp($char, "\n") == 0) {
                            error_func(1);
                        }
                        else {
                            $main_ptr++;
                            $side_ptr = 0;
                            $state = state::TYP_CHECK;
                            $state = state::OP_VS;
                        }
                        break;
                    case 5: // radek konci
                        if (strcmp($char,"#") == 0){
                            $state = state::COMMENT;
                        }
                        elseif ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            continue;
                        }
                        elseif (strcmp($char,"\n") == 0){
                            $state = state::IDLE;
                            $status = true;
                        }
                        else {
                            error_func(1);
                        }
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::OP_VSS:
                switch ($main_ptr) {
                    case 0: //alespon jeden bily znak
                    case 2:
                    case 4:
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $main_ptr++
                        }
                        else { error_func(1);}
                        break;
                    case 1: //dalsi bily znak nebo destination
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            continue;
                        }
                        elseif (strcmp($char, "\n") == 0) {
                            error_func(1);
                        }
                        else {
                            $main_ptr++;
                            $side_ptr = 0;
                            $state = state::DES_CHECK;
                            $state = state::OP_VSS;
                        }
                        break;
                    case 3:
                    case 5://dalsi bily znak nebo source
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            continue;
                        }
                        elseif (strcmp($char, "\n") == 0) {
                            error_func(1);
                        }
                        else {
                            $main_ptr++;
                            $side_ptr = 0;
                            $state = state::SYM_CHECK;
                            $state = state::OP_VSS;
                        }
                        break;
                    case 6: // radek konci
                        if (strcmp($char,"#") == 0){
                            $state = state::COMMENT;
                        }
                        elseif ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            continue;
                        }
                        elseif (strcmp($char,"\n") == 0){
                            $state = state::IDLE;
                            $status = true;
                        }
                        else {
                            error_func(1);
                        }
                        break;
                    default:
                        error_func(1);                
                }
                break;

            case state::OP_LSS:
                switch ($main_ptr) {
                    case 0: //alespon jeden bily znak
                    case 2:
                    case 4:
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $main_ptr++
                        }
                        else { error_func(1);}
                        break;
                    case 1: //dalsi bily znak nebo destination
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            continue;
                        }
                        elseif (strcmp($char, "\n") == 0) {
                            error_func(1);
                        }
                        else {
                            $main_ptr++;
                            $side_ptr = 0;
                            $state = state::LAB_CHECK;
                            $state = state::OP_VSS;
                        }
                        break;
                    case 3:
                    case 5://dalsi bily znak nebo source
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            continue;
                        }
                        elseif (strcmp($char, "\n") == 0) {
                            error_func(1);
                        }
                        else {
                            $main_ptr++;
                            $side_ptr = 0;
                            $state = state::SYM_CHECK;
                            $state = state::OP_VSS;
                        }
                        break;
                    case 6: // radek konci
                        if (strcmp($char,"#") == 0){
                            $state = state::COMMENT;
                        }
                        elseif ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            continue;
                        }
                        elseif (strcmp($char,"\n") == 0){
                            $state = state::IDLE;
                            $status = true;
                        }
                        else {
                            error_func(1);
                        }
                        break;
                    default:
                        error_func(1);                
                }
                break;

            case state::ADD_AND:
                switch ($main_ptr) {
                    case 1:
                        if ((strcmp($char, "D") == 0)||(strcmp($char, "N"))){
                            $main_ptr++;
                        }
                        else { error_func(1);}
                        break;
                    case 2: // ADD or AND
                        if (strcmp($char, "D") == 0){
                            $main_ptr = 0;
                            $state = state::OP_VSS;
                        }
                        else { error_func(1);}
                        break;
                    default:
                        error_func(1);
                    }
                break;

            case state::SUB:
                switch ($main_ptr) {
                    case 2: // SUB
                        if (strcmp($char, "B") == 0){
                            $main_ptr = 0;
                            $state = state::OP_VSS;
                        }
                        else { error_func(1);}
                        break;
                    default:
                        error_func(1);
                    }
                }
                break;
            
            case state::I:
                if (strcmp($char, "D") == 0){
                    $main_ptr++;
                    $state = state::IDIV;
                }
                elseif (strcmp($char, "N") == 0){
                    $main_ptr++;
                    $state = state::INT2CHAR;
                }
                else {
                    error_func(1);
                }
                break;

            case state::IDIV:
                switch ($main_ptr) {
                    case 2:
                        if (strcmp($char, "I") == 0){
                            $main_ptr++;
                        }
                        else { error_func(1);}
                        break;
                    case 3: // IDIV
                        if (strcmp($char, "V") == 0){
                            $main_ptr = 0;
                            $state = state::OP_VSS;
                        }
                        else { error_func(1);}
                        break;
                    default:
                        error_func(1);
                    }
                break;
                

            case state::INT2CHAR:
                switch ($main_ptr) {
                    case 2:
                        if (strcmp($char, "T") == 0){ $main_ptr++; }
                        else { error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char, "2") == 0){ $main_ptr++; }
                        else { error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char, "C") == 0){ $main_ptr++; }
                        else { error_func(1);}
                        break;
                    case 5:
                        if (strcmp($char, "H") == 0){ $main_ptr++; }
                        else { error_func(1);}
                        break;
                    case 6:
                        if (strcmp($char, "A") == 0){ $main_ptr++; }
                        else { error_func(1);}
                        break;
                    case 7: // INT2CHAR
                        if (strcmp($char, "R") == 0){ 
                            $main_ptr = 0;
                            $state = state::OP_VS;
                        }
                        else { error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::M:
                if (strcmp($char, "U") == 0){
                    $main_ptr++;
                    $state = state::MUL;
                }
                elseif (strcmp($char, "O") == 0) {
                    $main_ptr++;
                    $state = state::MUL;
                }
                else {
                    error_func(1);
                }
                break;

            case state::MUL:
                if (strcmp($char, "L") == 0){
                    $main_ptr = 0;
                    $state = state::OP_VSS;
                }
                else { error_func(1);}
                break;

            case state::MOVE:
                switch ($main_ptr) {
                    case 2:
                        if (strcmp($char, "V") == 0){
                            $main_ptr++;
                        }
                        else { error_func(1)};
                        break;
                    case 3:// MOVE
                        if (strcmp($char, "E") == 0){
                            $main_ptr = 0;
                            $state = state::OP_VS;
                        }
                        else { error_func(1)};
                        break;
                    default:
                        error_func(1);
                }
                break;


            case state::J:
                switch($main_ptr){
                    case 1:
                        if (strcmp($char,"U") == 0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 2:
                        if (strcmp($char,"M") == 0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"P") == 0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"I") == 0){ //JUMPI
                            $main_ptr++;
                        }
                        elseif ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)) { //JUMP_
                            $side_ptr = 0;
                            $called_from = state::ROW_END;
                            $state = state::LAB_CHECK;
                        }
                        else {error_func(1);}
                        break;
                    case 5:
                        if (strcmp($char,"F") == 0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 6:
                        if (strcmp($char,"E") == 0){
                            $main_ptr++;
                            $state = state::JUMPIFEQ;
                        }
                        elseif (strcmp($char,"N") == 0){
                            $main_ptr++;
                            $state = state::JUMPIFNEQ;
                        }
                        else {error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;
            
            case state::JUMPIFEQ:
                if (strcmp($char,"Q") == 0){
                    $main_ptr = 0;
                    $state = state::OP_LSS;
                }
                else {
                    error_func(1);
                }
                break;
            
            case state::JUMPIFNEQ:
                switch ($main_ptr) {
                    case 7:
                        if (strcmp($char,"E") == 0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 8:
                        if (strcmp($char,"Q") == 0){
                            $main_ptr = 0;
                            $state = state::OP_LSS;
                        }
                        else {
                            error_func(1);
                        }
                        break;
                    default:
                        error_func(1);
                }
                break;

            
            case state::C:
                if (strcmp($char,"R")){
                    $main_ptr++;
                    $state == state::CREATEFRAME;
                }
                elseif (strcmp($char,"A")){
                    $main_ptr++;
                    $state == state::CALL;
                }
                elseif (strcmp($char,"O")){
                    $main_ptr++;
                    $state = state::CONCAT;
                }
                else {
                    error_func(1);
                }
                break;

            case state::CREATEFRAME:
                switch ($main_ptr) {
                    case 2:
                    case 5:
                        if (strcmp($char,"E")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                    case 8:
                        if (strcmp($char,"A")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"T")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 6:
                        if (strcmp($char,"F")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 7:
                        if (strcmp($char,"R")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 9:
                        if (strcmp($char,"M")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 10:
                        if (strcmp($char,"E")==0){
                            $main_ptr++;
                            $state = state::ROW_END;
                        }
                        else {error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::CALL:
                switch ($main_ptr){
                    case 2:
                        if (strcmp($char,"L")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"L")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $side_ptr = 0;
                            $state = state::LAB_CHECK;
                            $called_from = state::ROW_END;
                        }
                        else { error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::CONCAT:
                switch ($main_ptr) {
                    case 2:
                        if (strcmp($char,"N")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"C")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"A")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 5:
                        if (strcmp($char,"T")==0){
                            $main_ptr = 0;
                            $state = state::OP_VSS;
                        }
                        else {error_func(1);}
                        break;
                    default:
                        error_func(1);                    
                }
                break;

            case state::P:
                if (strcmp($char,"O")==0){
                    $main_ptr++;
                    $state = state::PO;
                }
                elseif (strcmp($char,"U")==0){
                    $main_ptr++;
                    $state = state::PU;
                }
                else {
                    error_func(1);
                }
                break;

            case state::PO:
                switch($main_ptr){
                    case 2:
                        if (strcmp($char,"P")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"F")==0){
                            $main_ptr++;
                            $state = state::P_FRAME;
                        }
                        elseif (strcmp($char,"S")){
                            $main_ptr++;
                        }
                        else {
                            error_func(1);
                        }
                        break;
                    case 4: // POPS_
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $side_ptr = 0;
                            $state = state::VAR_CHECK;
                            $called_from = state::ROW_END;
                        }
                        else { error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::PU:
                switch($main_ptr){
                    case 2:
                        if (strcmp($char,"S")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"H")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"F")==0){
                            $main_ptr++;
                            $state = state::P_FRAME;
                        }
                        elseif (strcmp($char,"S")){
                            $main_ptr++;
                        }
                        else {
                            error_func(1);
                        }
                        break;
                    case 4: // PUSH_
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $side_ptr = 0;
                            $state = state::SYM_CHECK;
                            $called_from = state::ROW_END;
                        }
                        else { error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::P_FRAME:
                switch($main_ptr){
                    case 4:
                        if (strcmp($char,"R")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 5:
                        if (strcmp($char,"A")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 6:
                        if (strcmp($char,"M")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 7:
                        if (strcmp($char,"E")==0){
                            $main_ptr = 0;
                            $state = state::ROW_END;
                        }
                        else {error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::D:
                if (strcmp($char,"E")==0){ //DEFVAR
                    $main_ptr++;
                    $state = state::DEFVAR;
                }
                elseif (strcmp($char,"P")==0) { //DPRINT
                    $main_ptr++;
                    $state = state::DPRINT;
                }
                else {
                    error_func(1);
                }
                break;

            case state::DEFVAR:
                switch ($main_ptr){
                    case 2:
                        if (strcmp($char,"F")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"V")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"A")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 5:
                        if (strcmp($char,"R")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 6:
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $side_ptr = 0;
                            $state = state::VAR_CHECK;
                            $called_from = state::ROW_END;
                        }
                        else { error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::DPRINT:
                switch($main_ptr){
                    case 2:
                        if (strcmp($char,"R")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"I")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"N")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 5:
                        if (strcmp($char,"T")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 6:
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $side_ptr = 0;
                            $state = state::SYM_CHECK;
                            $called_from = state::ROW_END;
                        }
                        else { error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::R:
                switch ($main_ptr) {
                    case 1:
                        if (strcmp($char,"E")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 2:
                        if (strcmp($char,"T")==0){
                            $main_ptr++;
                            $state = state::RETURN;
                        }
                        elseif (strcmp($char,"A")==0){
                            $main_ptr++;
                            $state = state::READ;
                        }
                        else {
                            error_func(1);
                        }
                        break;
                    default:
                        
                }
                break;

            case state::RETURN:
                switch ($main_ptr){
                    case 3:
                        if (strcmp($char,"U")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"R")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 5:
                        if (strcmp($char,"N")==0){
                            $state = state::ROW_END;
                        }
                        else {error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::READ:
                switch ($main_ptr){
                    case 3:
                        if (strcmp($char,"A")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"D")==0){
                            $main_ptr = 0;
                            $state = state::OP_VT;
                        }
                        else {error_func(1);}
                        break;
                }
                break;
            
            case state::LABEL:
                switch ($main_ptr){
                    case 1:
                        if (strcmp($char,"A")==0) { //LABEL
                            $main_ptr++;
                        }
                        elseif (strcmp($char,"T")==0) { //LT
                            $main_ptr = 0;
                            $state = state::OP_VSS;
                        }
                        else {
                            error_func(1);
                        }
                        break;
                    case 2:
                        if (strcmp($char,"B")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"E")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"L")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 5:
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $side_ptr = 0;
                            $state = state::LAB_CHECK;
                            $called_from = state::ROW_END;
                        }
                        else { error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::GETCHAR:
                switch ($main_ptr) {
                    case 1:
                        if (strcmp($char,"E")==0) { //GETCHAR
                            $main_ptr++;
                        }
                        elseif (strcmp($char,"T")==0) { //GT
                            $main_ptr = 0;
                            $state = state::OP_VSS;
                        }
                        else {
                            error_func(1);
                        }
                        break;
                    case 2:
                        if (strcmp($char,"T")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"C")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"H")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 5:
                        if (strcmp($char,"A")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 6:
                        if (strcmp($char,"R")==0){
                            $main_ptr = 0;
                            $state = state::OS_VSS;
                        }
                        else {error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::EXIT:
                switch ($main_ptr) {
                    case 1:
                        if (strcmp($char,"X")==0) { //EXIT
                            $main_ptr++;
                        }
                        elseif (strcmp($char,"Q")==0) { //EQ
                            $main_ptr = 0;
                            $state = state::OP_VSS;
                        }
                        else {
                            error_func(1);
                        }
                        break;
                    case 2:
                        if (strcmp($char,"I")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"T")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $side_ptr = 0;
                            $state = state::SYM_CHECK;
                            $called_from = state::ROW_END;
                        }
                        else { error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::OR:
                if (strcmp($char,"R")==0){
                    $main_ptr = 0;
                    $state = state::OP_VSS;
                }
                else {
                    error_func(1);
                }
                break;

            case state::NOT:
                switch ($main_ptr) {
                    case 1:
                        if (strcmp($char,"O")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 2:
                        if (strcmp($char,"T")==0){
                            $main_ptr = 0;
                            $state = state::OP_VSS;
                        }
                        else {error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::S:
                switch ($main_ptr) {
                    case 1:
                        if (strcmp($char,"T")==0){
                            $main_ptr++;
                        }
                        elseif (strcmp($char,"E")==0){
                            $main_ptr++;
                            $state = state::SETCHAR;
                        }
                        elseif (strcmp($char,"U")==0){
                            $main_ptr++;
                            $state = state::SUB;
                        }
                        else {error_func(1);}
                        break;
                    case 2:
                        if (strcmp($char,"R")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"L")==0){
                            $main_ptr++;
                            $state = state::STRLEN;
                        }
                        elseif (strcmp($char,"I")==0){
                            $main_ptr++;
                            $state = state::STRI2INT;
                        }
                        else {error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::SETCHAR:
                switch ($main_ptr) {
                    case 2:
                        if (strcmp($char,"T")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"C")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"H")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 5:
                        if (strcmp($char,"A")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 6:
                        if (strcmp($char,"R")==0){
                            $main_ptr=0;
                            $state = state::OP_VSS;
                        }
                        else {error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::STRLEN:
                switch ($main_ptr) {
                    case 4:
                        if (strcmp($char,"E")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 5:
                        if (strcmp($char,"R")==0){
                            $main_ptr = 0;
                            $state = state::OP_VS;
                        }
                        else {error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::STRI2INT:
                switch ($main_ptr) {
                    case 4:
                        if (strcmp($char,"2")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 5:
                        if (strcmp($char,"I")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 6:
                        if (strcmp($char,"N")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 7:
                        if (strcmp($char,"T")==0){
                            $main_ptr=0;
                            $state = state::OP_VSS;
                        }
                        else {error_func(1);}
                        break;
                }
                break;

            case state::WRITE:
                switch ($main_ptr) {
                    case 1:
                        if (strcmp($char,"R")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 2:
                        if (strcmp($char,"I")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"T")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"E")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 5:
                        if ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                            $side_ptr = 0;
                            $state = state::SYM_CHECK;
                            $called_from = state::ROW_END;
                        }
                        else { error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::TYPE:
                switch ($main_ptr) {
                    case 1:
                        if (strcmp($char,"Y")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 2:
                        if (strcmp($char,"P")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"E")==0){
                            $main_ptr = 0;
                            $state = state::OP_VS;
                        }
                        else {error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::BREAK:
                switch ($main_ptr) {
                    case 1:
                        if (strcmp($char,"R")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 2:
                        if (strcmp($char,"E")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 3:
                        if (strcmp($char,"A")==0){$main_ptr++;}
                        else {error_func(1);}
                        break;
                    case 4:
                        if (strcmp($char,"K")==0){
                            $main_ptr++;
                            $state = state::ROW_END;
                        }
                        else {error_func(1);}
                        break;
                    default:
                        error_func(1);
                }
                break;

            case state::DES_CHECK:
                switch ($side_ptr) {
                    case 0: //prvni znak musi byt nejaky
                        break;
                    default: //ostatni znaky temer libovolne
                        break;

                    // pri uspesne kontrole ===   $state = $called_from;
                }
                break;
            
            case state::SYM_CHECK:
                switch ($side_ptr) {
                    case 0: //prvni znak musi byt nejaky
                        break;
                    default: //ostatni znaky temer libovolne
                        break;

                    // pri uspesne kontrole ===   $state = $called_from;
                }
                break;
            
            case state::LAB_CHECK:
                switch ($side_ptr) {
                    case 0: //prvni znak musi byt nejaky
                        break;
                    default: //ostatni znaky temer libovolne
                        break;

                    // pri uspesne kontrole ===   $state = $called_from;
                }
                break;

            case state::ROW_END:
                if (strcmp($char,"#") == 0){
                    $state = state::COMMENT;
                }
                elseif ((strcmp($char," ") == 0)||(strcmp($char,"\t") == 0)){
                    continue;
                }
                elseif (strcmp($char,"\n") == 0){
                    $state = state::IDLE;
                    $status = true;
                }
                else {
                    error_func(1);
                }
                break;
        }
        echo $char;
        echo " OK\n";
    

    fclose($file);
?>